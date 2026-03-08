#include <QApplication>
#include <QFileDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QFrame>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <jack/jack.h>
#include <sndfile.h>

#include <atomic>
#include <cerrno>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
QString formatSeconds(double seconds)
{
    if (seconds < 0.0) {
        seconds = 0.0;
    }

    const int total = static_cast<int>(seconds + 0.5);
    const int mins = total / 60;
    const int secs = total % 60;
    return QString("%1:%2").arg(mins, 2, 10, QLatin1Char('0')).arg(secs, 2, 10, QLatin1Char('0'));
}
} // namespace

class JackWavPlayer {
public:
    JackWavPlayer()
    {
        jack_status_t status = static_cast<jack_status_t>(0);
        client_ = jack_client_open("rakplay", JackNullOption, &status);
        if (!client_) {
            throw std::runtime_error("Unable to connect to JACK server");
        }

        out_left_ = jack_port_register(client_, "out_l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
        out_right_ = jack_port_register(client_, "out_r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
        if (!out_left_ || !out_right_) {
            jack_client_close(client_);
            client_ = nullptr;
            throw std::runtime_error("Unable to create JACK output ports");
        }

        jack_set_process_callback(client_, &JackWavPlayer::processCallback, this);
        if (jack_activate(client_) != 0) {
            jack_client_close(client_);
            client_ = nullptr;
            throw std::runtime_error("Unable to activate JACK client");
        }

        sample_rate_.store(static_cast<double>(jack_get_sample_rate(client_)), std::memory_order_release);
    }

    ~JackWavPlayer()
    {
        if (client_) {
            jack_deactivate(client_);
            jack_client_close(client_);
            client_ = nullptr;
        }
    }

    bool loadWavFile(const QString& path, QString& error)
    {
        if (!client_) {
            error = "JACK client is not available";
            return false;
        }

        stop();
        jack_deactivate(client_);

        SF_INFO info{};
        SNDFILE* file = sf_open(path.toStdString().c_str(), SFM_READ, &info);
        if (!file) {
            error = QString("Unable to open file: %1").arg(sf_strerror(nullptr));
            jack_activate(client_);
            return false;
        }

        if ((info.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
            sf_close(file);
            error = "Only WAV files are supported";
            jack_activate(client_);
            return false;
        }

        if (info.frames <= 0 || info.channels < 1) {
            sf_close(file);
            error = "The selected WAV file has no playable audio data";
            jack_activate(client_);
            return false;
        }

        std::vector<float> interleaved(static_cast<size_t>(info.frames) * static_cast<size_t>(info.channels));
        sf_count_t read_frames = sf_readf_float(file, interleaved.data(), info.frames);
        sf_close(file);

        if (read_frames <= 0) {
            error = "Failed to read audio data from file";
            jack_activate(client_);
            return false;
        }

        const size_t frames = static_cast<size_t>(read_frames);
        std::vector<float> left(frames, 0.0f);
        std::vector<float> right(frames, 0.0f);

        for (size_t i = 0; i < frames; ++i) {
            const size_t base = i * static_cast<size_t>(info.channels);
            left[i] = interleaved[base];
            right[i] = (info.channels > 1) ? interleaved[base + 1] : interleaved[base];
        }

        left_ = std::move(left);
        right_ = std::move(right);
        frame_count_.store(frames, std::memory_order_release);
        position_.store(0, std::memory_order_release);
        loaded_path_ = path;

        jack_activate(client_);
        return true;
    }

    void play()
    {
        if (frame_count_.load(std::memory_order_acquire) == 0) {
            return;
        }
        paused_.store(false, std::memory_order_release);
        playing_.store(true, std::memory_order_release);
    }

    void pause()
    {
        if (playing_.load(std::memory_order_acquire)) {
            playing_.store(false, std::memory_order_release);
            paused_.store(true, std::memory_order_release);
        }
    }

    void stop()
    {
        playing_.store(false, std::memory_order_release);
        paused_.store(false, std::memory_order_release);
        position_.store(0, std::memory_order_release);
    }

    void rewind()
    {
        position_.store(0, std::memory_order_release);
    }

    bool isPlaying() const
    {
        return playing_.load(std::memory_order_acquire);
    }

    bool isPaused() const
    {
        return paused_.load(std::memory_order_acquire);
    }

    bool hasFile() const
    {
        return frame_count_.load(std::memory_order_acquire) > 0;
    }

    bool connectToRakarrack(QString& error)
    {
        const QStringList ports = listRakarrackInputPorts(error);
        if (ports.size() < 2) {
            return false;
        }

        return connectToPorts(ports[0], ports[1], error);
    }

    QStringList listRakarrackInputPorts(QString& error) const
    {
        if (!client_) {
            error = "JACK client is not available";
            return {};
        }

        const char** ports = jack_get_ports(client_, nullptr, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput);
        if (!ports) {
            error = "No JACK input ports are available";
            return {};
        }

        QStringList targets;
        for (size_t i = 0; ports[i] != nullptr; ++i) {
            const QString port_name = QString::fromUtf8(ports[i]);
            if (port_name.toLower().contains("rakarrack")) {
                targets.append(port_name);
            }
        }
        jack_free(ports);

        if (targets.size() < 2) {
            error = "Could not find two rakarrack input audio ports in JACK";
            return {};
        }

        return targets;
    }

    bool connectToPorts(const QString& left_target, const QString& right_target, QString& error)
    {
        if (!client_) {
            error = "JACK client is not available";
            return false;
        }

        if (left_target.isEmpty() || right_target.isEmpty()) {
            error = "Both left and right target ports must be selected";
            return false;
        }

        const char* left_out = jack_port_name(out_left_);
        const char* right_out = jack_port_name(out_right_);

        const std::string left = left_target.toStdString();
        const std::string right = right_target.toStdString();

        const int left_result = jack_connect(client_, left_out, left.c_str());
        if (left_result != 0 && left_result != EEXIST) {
            error = QString("Failed to connect left channel to %1").arg(left_target);
            return false;
        }

        const int right_result = jack_connect(client_, right_out, right.c_str());
        if (right_result != 0 && right_result != EEXIST) {
            error = QString("Failed to connect right channel to %1").arg(right_target);
            return false;
        }

        return true;
    }

    void setLoopEnabled(bool enabled)
    {
        loop_enabled_.store(enabled, std::memory_order_release);
    }

    bool isLoopEnabled() const
    {
        return loop_enabled_.load(std::memory_order_acquire);
    }

    size_t currentFrame() const
    {
        return position_.load(std::memory_order_acquire);
    }

    size_t totalFrames() const
    {
        return frame_count_.load(std::memory_order_acquire);
    }

    double sampleRate() const
    {
        return sample_rate_.load(std::memory_order_acquire);
    }

    QString loadedPath() const
    {
        return loaded_path_;
    }

private:
    static int processCallback(jack_nframes_t nframes, void* arg)
    {
        return static_cast<JackWavPlayer*>(arg)->process(nframes);
    }

    int process(jack_nframes_t nframes)
    {
        auto* out_l = static_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(out_left_, nframes));
        auto* out_r = static_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(out_right_, nframes));

        const size_t total = frame_count_.load(std::memory_order_acquire);
        size_t pos = position_.load(std::memory_order_acquire);
        const bool playing = playing_.load(std::memory_order_acquire);
        const bool loop = loop_enabled_.load(std::memory_order_acquire);

        for (jack_nframes_t i = 0; i < nframes; ++i) {
            if (playing && total > 0) {
                if (pos >= total) {
                    if (loop) {
                        pos = 0;
                    } else {
                        out_l[i] = 0.0f;
                        out_r[i] = 0.0f;
                        continue;
                    }
                }

                out_l[i] = left_[pos];
                out_r[i] = right_[pos];
                ++pos;
            } else {
                out_l[i] = 0.0f;
                out_r[i] = 0.0f;
            }
        }

        if (!loop && pos >= total) {
            playing_.store(false, std::memory_order_release);
            paused_.store(false, std::memory_order_release);
        }
        position_.store(pos, std::memory_order_release);

        return 0;
    }

    jack_client_t* client_ = nullptr;
    jack_port_t* out_left_ = nullptr;
    jack_port_t* out_right_ = nullptr;

    std::vector<float> left_;
    std::vector<float> right_;
    std::atomic<size_t> frame_count_{0};
    std::atomic<size_t> position_{0};
    std::atomic<double> sample_rate_{48000.0};
    std::atomic<bool> playing_{false};
    std::atomic<bool> paused_{false};
    std::atomic<bool> loop_enabled_{false};
    QString loaded_path_;
};

class RakPlayWindow : public QWidget {
public:
    explicit RakPlayWindow(JackWavPlayer& player, QWidget* parent = nullptr)
        : QWidget(parent), player_(player)
    {
        setWindowTitle("rakplay");
        setMinimumWidth(520);

        auto* main_layout = new QVBoxLayout(this);
        auto* file_title = new QLabel("Loaded WAV file:");
        file_name_label_ = new QLabel("(none)");
        file_name_label_->setFrameShape(QFrame::Panel);
        file_name_label_->setFrameShadow(QFrame::Sunken);
        file_name_label_->setMinimumHeight(28);

        auto* status_title = new QLabel("Status:");
        status_label_ = new QLabel("Stopped");
        auto* time_title = new QLabel("Position:");
        time_label_ = new QLabel("00:00 / 00:00");

        auto* controls = new QHBoxLayout();
        auto* load_button = new QPushButton("Load WAV...");
        auto* play_button = new QPushButton("Play");
        auto* pause_button = new QPushButton("Pause");
        auto* stop_button = new QPushButton("Stop");
        auto* rewind_button = new QPushButton("Rewind");
        auto* loop_button = new QPushButton("Loop");
        auto* connect_button = new QPushButton("Connect to rakarrack");
        loop_button->setCheckable(true);

        controls->addWidget(load_button);
        controls->addWidget(play_button);
        controls->addWidget(pause_button);
        controls->addWidget(stop_button);
        controls->addWidget(rewind_button);
        controls->addWidget(loop_button);
        controls->addWidget(connect_button);

        main_layout->addWidget(file_title);
        main_layout->addWidget(file_name_label_);
        main_layout->addWidget(status_title);
        main_layout->addWidget(status_label_);
        main_layout->addWidget(time_title);
        main_layout->addWidget(time_label_);
        main_layout->addLayout(controls);

        connect(load_button, &QPushButton::clicked, this, [this]() { loadFile(); });
        connect(play_button, &QPushButton::clicked, this, [this]() {
            player_.play();
            refreshStatus();
        });
        connect(pause_button, &QPushButton::clicked, this, [this]() {
            player_.pause();
            refreshStatus();
        });
        connect(stop_button, &QPushButton::clicked, this, [this]() {
            player_.stop();
            refreshStatus();
        });
        connect(rewind_button, &QPushButton::clicked, this, [this]() {
            player_.rewind();
            refreshStatus();
        });
        connect(loop_button, &QPushButton::toggled, this, [this](bool enabled) {
            player_.setLoopEnabled(enabled);
            refreshStatus();
        });
        connect(connect_button, &QPushButton::clicked, this, [this]() {
            QString error;
            const QStringList ports = player_.listRakarrackInputPorts(error);
            if (ports.size() < 2) {
                QMessageBox::warning(this, "rakplay", error);
                return;
            }

            QDialog dialog(this);
            dialog.setWindowTitle("Connect to rakarrack");

            auto* layout = new QVBoxLayout(&dialog);
            auto* form = new QFormLayout();
            auto* left_combo = new QComboBox(&dialog);
            auto* right_combo = new QComboBox(&dialog);
            left_combo->addItems(ports);
            right_combo->addItems(ports);
            if (ports.size() > 1) {
                right_combo->setCurrentIndex(1);
            }

            form->addRow("Left output ->", left_combo);
            form->addRow("Right output ->", right_combo);
            layout->addLayout(form);

            auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
            layout->addWidget(buttons);

            if (dialog.exec() != QDialog::Accepted) {
                return;
            }

            QString connect_error;
            if (player_.connectToPorts(left_combo->currentText(), right_combo->currentText(), connect_error)) {
                QMessageBox::information(this, "rakplay", "Connected rakplay outputs to rakarrack inputs.");
            } else {
                QMessageBox::warning(this, "rakplay", connect_error);
            }
        });

        auto* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this]() { refreshStatus(); });
        timer->start(100);
    }

private:
    void loadFile()
    {
        const QString path = QFileDialog::getOpenFileName(
            this,
            "Open WAV file",
            QString(),
            "WAV files (*.wav *.WAV)");

        if (path.isEmpty()) {
            return;
        }

        QString error;
        if (!player_.loadWavFile(path, error)) {
            QMessageBox::critical(this, "Load error", error);
            return;
        }

        file_name_label_->setText(path);
        status_label_->setText("Stopped");
        time_label_->setText("00:00 / 00:00");
    }

    void refreshStatus()
    {
        if (!player_.hasFile()) {
            status_label_->setText("No file loaded");
            time_label_->setText("00:00 / 00:00");
            return;
        }

        const double sample_rate = player_.sampleRate();
        const double current_seconds = static_cast<double>(player_.currentFrame()) / sample_rate;
        const double total_seconds = static_cast<double>(player_.totalFrames()) / sample_rate;
        time_label_->setText(QString("%1 / %2")
                                 .arg(formatSeconds(current_seconds))
                                 .arg(formatSeconds(total_seconds)));

        if (player_.isPlaying()) {
            status_label_->setText(player_.isLoopEnabled() ? "Playing (Loop)" : "Playing");
            return;
        }

        if (player_.isPaused()) {
            status_label_->setText("Paused");
            return;
        }

        status_label_->setText("Stopped");
    }

    JackWavPlayer& player_;
    QLabel* file_name_label_ = nullptr;
    QLabel* status_label_ = nullptr;
    QLabel* time_label_ = nullptr;
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    try {
        JackWavPlayer player;
        RakPlayWindow window(player);
        window.show();
        return app.exec();
    } catch (const std::exception& ex) {
        QMessageBox::critical(nullptr, "rakplay", ex.what());
        return 1;
    }
}
