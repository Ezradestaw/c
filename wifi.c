
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

#define SAMPLE_INTERVAL 5
#define DEFAULT_TARGET "1.1.1.1"
#define LOG_FILE "wifi_lab.csv"

static volatile sig_atomic_t running = 1;

typedef struct {
    int signal;
    int channel;
    int frequency;
    double latency_ms;
    int ping_ok;
} Measurement;

static void stop_handler(int sig)
{
    (void)sig;
    running = 0;
}

static void get_timestamp(char *buf, size_t size)
{
    time_t now = time(NULL);
    struct tm tm_now;

    if (localtime_r(&now, &tm_now))
        strftime(buf, size, "%Y-%m-%d %H:%M:%S", &tm_now);
    else
        snprintf(buf, size, "unknown");
}

/* Run a command and capture its standard output. */
static int run_command(const char *command,
                       char *output, size_t size)
{
    FILE *pipe = popen(command, "r");
    if (!pipe)
        return -1;

    size_t used = 0;
    output[0] = '\0';

    while (used + 1 < size) {
        if (!fgets(output + used, (int)(size - used), pipe))
            break;
        used = strlen(output);
    }

    int status = pclose(pipe);
    if (status == -1)
        return -1;

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return -1;
}

static void read_wifi(Measurement *m, const char *iface)
{
    char command[256];
    char output[8192];

    m->signal = -1;
    m->channel = -1;
    m->frequency = -1;

    snprintf(command, sizeof(command),
             "iw dev %s link 2>/dev/null", iface);

    if (run_command(command, output, sizeof(output)) != 0)
        return;

    char *line = strtok(output, "\n");

    while (line) {
        int value;

        if (sscanf(line, " freq: %d", &value) == 1)
            m->frequency = value;

        if (sscanf(line, " signal: %d dBm", &value) == 1)
            m->signal = value;

        line = strtok(NULL, "\n");
    }

    if (m->frequency > 0) {
        /* Common 2.4 and 5 GHz channel mapping. */
        if (m->frequency == 2484)
            m->channel = 14;
        else if (m->frequency >= 2412 &&
                 m->frequency <= 2472)
            m->channel = (m->frequency - 2407) / 5;
        else if (m->frequency >= 5000 &&
                 m->frequency <= 5895)
            m->channel = (m->frequency - 5000) / 5;
        else if (m->frequency >= 5955 &&
                 m->frequency <= 7115)
            m->channel = (m->frequency - 5950) / 5;
    }
}

static void read_latency(Measurement *m, const char *target)
{
    char command[256];
    char output[4096];

    m->latency_ms = -1.0;
    m->ping_ok = 0;

    snprintf(command, sizeof(command),
             "ping -n -c 1 -W 2 %s 2>/dev/null", target);

    if (run_command(command, output, sizeof(output)) != 0)
        return;

    char *p = strstr(output, "time=");
    if (p) {
        double latency;

        if (sscanf(p + 5, "%lf", &latency) == 1) {
            m->latency_ms = latency;
            m->ping_ok = 1;
        }
    }
}

int main(int argc, char **argv)
{
    const char *iface = argc > 1 ? argv[1] : "wlan0";
    const char *target = argc > 2 ? argv[2] : DEFAULT_TARGET;

    FILE *csv = fopen(LOG_FILE, "a");
    if (!csv) {
        perror("Opening CSV");
        return EXIT_FAILURE;
    }

    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);

    if (ftell(csv) == 0)
        fprintf(csv,
            "timestamp,interface,frequency_mhz,channel,"
            "signal_dbm,target,latency_ms,ping_success\n");

    unsigned long samples = 0;
    unsigned long successful = 0;
    double latency_sum = 0.0;

    printf("Wi-Fi lab monitor\nInterface: %s\nTarget: %s\n",
           iface, target);
    printf("CSV log: %s\nPress Ctrl+C to stop.\n\n", LOG_FILE);

    while (running) {
        Measurement m = {0};
        char timestamp[64];

        get_timestamp(timestamp, sizeof(timestamp));
        read_wifi(&m, iface);
        read_latency(&m, target);

        printf("[%s] freq=%d MHz channel=%d "
               "signal=%d dBm latency=",
               timestamp, m.frequency, m.channel, m.signal);

        if (m.ping_ok) {
            printf("%.2f ms\n", m.latency_ms);
            successful++;
            latency_sum += m.latency_ms;
        } else {
            printf("unavailable\n");
        }

        fprintf(csv, "%s,%s,%d,%d,%d,%s,",
                timestamp, iface, m.frequency, m.channel,
                m.signal, target);

        if (m.ping_ok)
            fprintf(csv, "%.3f,1\n", m.latency_ms);
        else
            fprintf(csv, ",0\n");

        fflush(csv);
        samples++;

        sleep(SAMPLE_INTERVAL);
    }

    printf("\n--- Experiment summary ---\n");
    printf("Samples: %lu\n", samples);
    printf("Successful ping samples: %lu\n", successful);

    if (successful)
        printf("Mean successful latency: %.2f ms\n",
               latency_sum / successful);

    printf("Results saved in %s\n", LOG_FILE);
    fclose(csv);

    return EXIT_SUCCESS;
}
