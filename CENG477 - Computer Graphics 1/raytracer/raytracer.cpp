#include "parser.h"
#include "ppm.h"
#include <thread>

#include "auxilary_functions.h"
#include "rendering.h"
using namespace std;

int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    Scene scene;
    scene.loadFromXml(argv[1]);

    for (int cameraID=0; cameraID < scene.cameras.size(); cameraID++)
    {
        // Get camera and calculate related vectors
        Camera & camera = scene.cameras[cameraID];
        camera.gaze = make_unit_vector(camera.gaze);
        camera.up = make_unit_vector(camera.up);
        camera.right = make_unit_vector(cross_product(camera.gaze, camera.up));
        camera.image_middle_point =
            vector_addition(camera.position,
                scalar_multiplication(
                    camera.gaze,
                    camera.near_distance));
        camera.image_top_left = vector_addition(
            scalar_multiplication(
                camera.right,
                camera.near_plane.x),
            vector_addition(
                camera.image_middle_point,
                scalar_multiplication(
                    camera.up,
                    camera.near_plane.w)));

        // Calculate the difference between each pixel
        float leftmost_position = camera.near_plane.x;
        float rightmost_position = camera.near_plane.y;
        float bottommost_position = camera.near_plane.z;
        float topmost_position = camera.near_plane.w;
        camera.u_factor = (rightmost_position - leftmost_position) / camera.image_width;
        camera.v_factor = (topmost_position - bottommost_position) / camera.image_height;

        // Divide the image into columns for rendering in parallel
        const int available_threads = static_cast<int>(thread::hardware_concurrency());
        const int sectionHeight = camera.image_height / available_threads;

        // create threads by giving them to the same image plane
        vector<thread> threads;
        auto* image = new unsigned char [camera.image_width * camera.image_height * 3];
        for (int i = 0; i < available_threads; i++)
        {
            int startRow = i * sectionHeight;
            int endRow = (i == available_threads - 1) ? camera.image_height : startRow + sectionHeight;
            threads.emplace_back(render, image, startRow, endRow, camera.image_width, camera, scene);
        }

        // Wait for all threads to finish
        for (auto &thread : threads)
        {
            thread.join();
        }
        // Write the ppm file
        write_ppm(camera.image_name.c_str(), image, camera.image_width, camera.image_height);

        delete[] image;
    }
}
