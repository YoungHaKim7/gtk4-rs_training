module;
#include <pthread.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

export module capture.app;

import capture.gui;
import capture.cameras;

namespace app_state {
    static volatile int running = 1;
    static int pic = 0;
    static int isFront = 1;
    static CameraSystem cameraSystem{};
}
using namespace app_state;

static void* camera_logic(void* /*arg*/)
{
    while (running == 1) {
        if (digitalRead(4) == 0) {
            printf("taking pics of %s item %i\n", (isFront == 1) ? "front" : "back", pic);
            for (int i = 0; i < cameraSystem.count; i++) {
                char buf[256];
                snprintf(buf, sizeof(buf), "%s/cam-%i_%i_%s.jpg", gui_get_working_dir(), i + 1, pic,
                         (isFront == 1) ? "a_front" : "b_back");
                capture_photo_to_path(cameraSystem.cameras[i], buf, cameraSystem.context);
                gui_set_image(isFront == 1, i, buf);
            }
            if (isFront == 1) isFront = 0; else { isFront = 1; pic += 1; }
            printf("pics taken...\n");
        }
    }
    return nullptr;
}

export int app_main(int /*argc*/, char** /*argv*/)
{
    system("pkill -f gphoto2");

    wiringPiSetupGpio();
    pinMode(4, INPUT);

    cameraSystem.context = gp_context_new();
    if (detect_and_open_cameras(cameraSystem) < GP_OK) {
        fprintf(stderr, "Failed to detect/open cameras. Exiting.\n");
        return 1;
    }

    pthread_t logic_thread_handle, gui_thread_handle;
    pthread_create(&logic_thread_handle, NULL, camera_logic, NULL);
    pthread_create(&gui_thread_handle, NULL, gui_thread_entry, NULL);
    pthread_join(gui_thread_handle, 0);
    pthread_join(logic_thread_handle, 0);

    for (int i = 0; i < cameraSystem.count; i++) {
        gp_camera_exit(cameraSystem.cameras[i], cameraSystem.context);
        gp_camera_free(cameraSystem.cameras[i]);
    }
    return 0;
}
