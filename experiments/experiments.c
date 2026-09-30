#include "../src/compressor/compressor.h"
#include "../src/decompressor/decompressor.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define EXPERIMENT_REPETITIONS 10
#define RESULT_FILE "experiments/results/results.csv"
#define SUMMARY_FILE "experiments/results/summary.csv"
#define ARCHIVE_FILE "experiments/results/temp.bin"
#define RESTORED_FILE "experiments/results/temp_restored"

typedef struct {
        const char* type;
        const char* filename;
} Experiment;

typedef struct {
        int count;

        double compression_ratio_mean;
        double compression_ratio_m2;

        double compression_time_mean;
        double compression_time_m2;

        double decompression_time_mean;
        double decompression_time_m2;

        long long original_size;
        long long compressed_size;

        int successful_runs;
} ExperimentStatistics;

static double get_time(void) {    // Get current time for measurements
        struct timespec time;
        timespec_get(&time, TIME_UTC);

        return (double)time.tv_sec + (double)time.tv_nsec / 1000000000.0;
}

static long long get_file_size(const char* filename) {    // Get file size
        FILE* file = fopen(filename, "rb");

        if (!file)
                return -1;

        fseek(file, 0, SEEK_END);
        long long size = ftell(file);
        fclose(file);

        return size;
}

static int compare_files(const char* first_filename, const char* second_filename) {    // Compare two files
        FILE* first = fopen(first_filename, "rb");
        FILE* second = fopen(second_filename, "rb");

        if (!first || !second) {
                if (first)
                        fclose(first);

                if (second)
                        fclose(second);

                return 0;
        }

        unsigned char first_buffer[4096];
        unsigned char second_buffer[4096];

        int equal = 1;

        while (1) {
                size_t first_read = fread(first_buffer, 1, sizeof(first_buffer), first);
                size_t second_read = fread(second_buffer, 1, sizeof(second_buffer), second);

                if (first_read != second_read) {
                        equal = 0;
                        break;
                }

                if (first_read == 0)
                        break;

                if (memcmp(first_buffer, second_buffer, first_read) != 0) {
                        equal = 0;
                        break;
                }
        }

        fclose(first);
        fclose(second);

        return equal;
}

static void update_statistics(ExperimentStatistics* statistics, double compression_ratio, double compression_time,
                              double decompression_time, long long original_size,
                              long long compressed_size) {    // Update experiment statistics
        double delta;
        double delta2;

        statistics->count++;

        delta = compression_ratio - statistics->compression_ratio_mean;
        statistics->compression_ratio_mean += delta / statistics->count;

        delta2 = compression_ratio - statistics->compression_ratio_mean;
        statistics->compression_ratio_m2 += delta * delta2;

        delta = compression_time - statistics->compression_time_mean;
        statistics->compression_time_mean += delta / statistics->count;

        delta2 = compression_time - statistics->compression_time_mean;
        statistics->compression_time_m2 += delta * delta2;

        delta = decompression_time - statistics->decompression_time_mean;
        statistics->decompression_time_mean += delta / statistics->count;

        delta2 = decompression_time - statistics->decompression_time_mean;
        statistics->decompression_time_m2 += delta * delta2;
        statistics->original_size = original_size;

        statistics->compressed_size = compressed_size;
        statistics->successful_runs++;
}

static double get_standard_deviation(double m2, int count) {    // Calculate standard deviation
        if (count < 2)
                return 0.0;

        return sqrt(m2 / (count - 1));
}

static int run_experiment(const Experiment* experiment, FILE* result_file, ExperimentStatistics* statistics,
                          int run) {    // Run one experiment
        double compression_start;
        double compression_end;
        double decompression_start;
        double decompression_end;

        long long original_size;
        long long compressed_size;

        double compression_time;
        double decompression_time;
        double compression_ratio;

        int compression_result;
        int decompression_result;
        int files_are_equal;

        original_size = get_file_size(experiment->filename);

        if (original_size < 0) {
                printf("Error: cannot open '%s'\n", experiment->filename);
                fprintf(result_file, "%s,%s,%d,-1,-1,0.0,0.0,0.0,FAILED\n", experiment->type, experiment->filename,
                        run);

                return 0;
        }

        remove(ARCHIVE_FILE);
        remove(RESTORED_FILE);

        compression_start = get_time();
        compression_result = compress(experiment->filename, ARCHIVE_FILE);
        compression_end = get_time();
        compression_time = compression_end - compression_start;

        if (compression_result != 0) {
                printf("Compression failed: %s\n", experiment->filename);
                fprintf(result_file, "%s,%s,%d,%lld,-1,0.0,%.6f,0.0,FAILED\n", experiment->type, experiment->filename,
                        run, original_size, compression_time);

                return 0;
        }

        compressed_size = get_file_size(ARCHIVE_FILE);

        if (compressed_size < 0) {
                printf("Error: cannot read archive '%s'\n", ARCHIVE_FILE);
                fprintf(result_file, "%s,%s,%d,%lld,-1,0.0,%.6f,0.0,FAILED\n", experiment->type, experiment->filename,
                        run, original_size, compression_time);

                return 0;
        }

        decompression_start = get_time();
        decompression_result = decompress(ARCHIVE_FILE, RESTORED_FILE);
        decompression_end = get_time();
        decompression_time = decompression_end - decompression_start;

        if (decompression_result != 0) {
                printf("Decompression failed: %s\n", experiment->filename);
                fprintf(result_file, "%s,%s,%d,%lld,%lld,%.6f,%.6f,%.6f,FAILED\n", experiment->type,
                        experiment->filename, run, original_size, compressed_size,
                        (double)compressed_size / (double)original_size, compression_time, decompression_time);

                return 0;
        }

        files_are_equal = compare_files(experiment->filename, RESTORED_FILE);

        if (!files_are_equal) {
                printf("Verification failed: %s\n", experiment->filename);
                fprintf(result_file, "%s,%s,%d,%lld,%lld,%.6f,%.6f,%.6f,FAILED\n", experiment->type,
                        experiment->filename, run, original_size, compressed_size,
                        (double)compressed_size / (double)original_size, compression_time, decompression_time);

                return 0;
        }

        if (original_size != 0)
                compression_ratio = (double)compressed_size / (double)original_size;
        else
                compression_ratio = 0.0;

        update_statistics(statistics, compression_ratio, compression_time, decompression_time, original_size,
                          compressed_size);

        printf("%s, run %d/%d\n", experiment->type, run, EXPERIMENT_REPETITIONS);
        printf("  Original size:      %lld bytes\n", original_size);
        printf("  Compressed size:    %lld bytes\n", compressed_size);
        printf("  Compression ratio:  %.4f\n", compression_ratio);
        printf("  Compression time:   %.6f s\n", compression_time);
        printf("  Decompression time: %.6f s\n", decompression_time);
        printf("  Verification:       OK\n");
        printf("\n");
        fprintf(result_file, "%s,%s,%d,%lld,%lld,%.6f,%.6f,%.6f,OK\n", experiment->type, experiment->filename, run,
                original_size, compressed_size, compression_ratio, compression_time, decompression_time);

        return 1;
}

static void write_summary(FILE* summary_file, const Experiment* experiment,
                          const ExperimentStatistics* statistics) {    // Write experiment summary
        double compression_ratio_stddev;
        double compression_time_stddev;
        double decompression_time_stddev;

        compression_ratio_stddev = get_standard_deviation(statistics->compression_ratio_m2, statistics->count);
        compression_time_stddev = get_standard_deviation(statistics->compression_time_m2, statistics->count);
        decompression_time_stddev = get_standard_deviation(statistics->decompression_time_m2, statistics->count);

        fprintf(summary_file,
                "%s,%s,%d,%d,%lld,%lld,"
                "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
                experiment->type, experiment->filename, EXPERIMENT_REPETITIONS, statistics->successful_runs,
                statistics->original_size, statistics->compressed_size, statistics->compression_ratio_mean,
                compression_ratio_stddev, statistics->compression_time_mean, compression_time_stddev,
                statistics->decompression_time_mean, decompression_time_stddev);
}

int main(void) {    // Entry point
        const Experiment experiments[] = {{"text_small", "experiments/data/text/text_100kb.txt"},

                                          {"text_medium", "experiments/data/text/text_1mb.txt"},

                                          {"text_large", "experiments/data/text/text_10mb.txt"},

                                          {"repetitive_small", "experiments/data/repetitive/repetitive_100kb.txt"},

                                          {"repetitive_medium", "experiments/data/repetitive/repetitive_1mb.txt"},

                                          {"repetitive_large", "experiments/data/repetitive/repetitive_10mb.txt"},

                                          {"random_small", "experiments/data/random/random_100kb.bin"},

                                          {"random_medium", "experiments/data/random/random_1mb.bin"},

                                          {"random_large", "experiments/data/random/random_10mb.bin"},

                                          {"compressed_small", "experiments/data/compressed/text_100kb.bin"},

                                          {"compressed_medium", "experiments/data/compressed/text_1mb.bin"},

                                          {"compressed_large", "experiments/data/compressed/text_10mb.bin"}};

        size_t experiment_count = sizeof(experiments) / sizeof(experiments[0]);

        FILE* result_file;
        FILE* summary_file;

        result_file = fopen(RESULT_FILE, "w");

        if (!result_file) {
                printf("Error: cannot create '%s'\n", RESULT_FILE);

                return 1;
        }

        summary_file = fopen(SUMMARY_FILE, "w");

        if (!summary_file) {
                printf("Error: cannot create '%s'\n", SUMMARY_FILE);
                fclose(result_file);

                return 1;
        }

        fprintf(result_file, "type,filename,run,original_size,compressed_size,"
                             "compression_ratio,compression_time,"
                             "decompression_time,verification\n");

        fprintf(summary_file, "type,filename,repetitions,successful_runs,"
                              "original_size,compressed_size,"
                              "compression_ratio_mean,compression_ratio_stddev,"
                              "compression_time_mean,compression_time_stddev,"
                              "decompression_time_mean,decompression_time_stddev\n");

        for (size_t i = 0; i < experiment_count; i++) {
                ExperimentStatistics statistics = {0};

                for (int run = 1; run <= EXPERIMENT_REPETITIONS; run++)
                        run_experiment(&experiments[i], result_file, &statistics, run);

                write_summary(summary_file, &experiments[i], &statistics);
        }

        fclose(result_file);
        fclose(summary_file);

        remove(ARCHIVE_FILE);
        remove(RESTORED_FILE);

        printf("Results saved to %s\n", RESULT_FILE);
        printf("Summary saved to %s\n", SUMMARY_FILE);

        return 0;
}
