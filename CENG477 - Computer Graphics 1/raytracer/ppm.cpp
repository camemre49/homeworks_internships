#include "ppm.h"
#include <stdexcept>

void write_ppm(const char* filename, unsigned char* data, int width, int height)
{
    FILE *outfile;
    const char *directory = "./"; // replace with your directory name
    //const char *directory = "test_scenes/outputs/"; // replace with your directory name
    char filepath[256]; // buffer to hold the full path

    // Construct the full path
    snprintf(filepath, sizeof(filepath), "%s/%s", directory, filename);

    if ((outfile = fopen(filepath, "w")) == NULL) 
    {
        throw std::runtime_error("Error: The ppm file cannot be opened for writing.");
    }

    (void) fprintf(outfile, "P3\n%d %d\n255\n", width, height);

    unsigned char color;
    for (size_t j = 0, idx = 0; j < height; ++j)
    {
        for (size_t i = 0; i < width; ++i)
        {
            for (size_t c = 0; c < 3; ++c, ++idx)
            {
                color = data[idx];

                if (i == width - 1 && c == 2)
                {
                    (void) fprintf(outfile, "%d", color);
                }
                else
                {
                    (void) fprintf(outfile, "%d ", color);
                }
            }
        }

        (void) fprintf(outfile, "\n");
    }

    (void) fclose(outfile);
}
