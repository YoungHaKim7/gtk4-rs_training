module;
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-context.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

export module capture.cameras;

export struct CameraSystem {
    CameraList* list = nullptr;
    Camera** cameras = nullptr;
    GPContext* context = nullptr;
    const char *name = nullptr, *value = nullptr;
    int count = 0;
};

static int open_cam(Camera** camera, const char* model, const char* port, GPContext* context)
{
    GPPortInfoList* portinfolist = NULL;
    CameraAbilitiesList* abilities = NULL;
    int ret, m, p;
    CameraAbilities a;
    GPPortInfo pi;

    ret = gp_camera_new(camera);
    if (ret < GP_OK)
        return ret;

    if (!abilities) {
        ret = gp_abilities_list_new(&abilities);
        if (ret < GP_OK)
            return ret;
        ret = gp_abilities_list_load(abilities, context);
        if (ret < GP_OK)
            return ret;
    }

    m = gp_abilities_list_lookup_model(abilities, model);
    if (m < GP_OK)
        return ret;
    ret = gp_abilities_list_get_abilities(abilities, m, &a);
    if (ret < GP_OK)
        return ret;
    ret = gp_camera_set_abilities(*camera, a);
    if (ret < GP_OK)
        return ret;

    if (!portinfolist) {
        ret = gp_port_info_list_new(&portinfolist);
        if (ret < GP_OK)
            return ret;
        ret = gp_port_info_list_load(portinfolist);
        if (ret < 0)
            return ret;
        ret = gp_port_info_list_count(portinfolist);
        if (ret < 0)
            return ret;
    }

    p = gp_port_info_list_lookup_path(portinfolist, port);
    if (p < GP_OK) return p;

    int r = gp_port_info_list_get_info(portinfolist, p, &pi);
    if (r < GP_OK) return r;
    r = gp_camera_set_port_info(*camera, pi);
    if (r < GP_OK) return r;
    return GP_OK;
}

export int detect_and_open_cameras(CameraSystem& sys)
{
    int ret = gp_list_new(&sys.list);
    if (ret < GP_OK) return ret;
    gp_list_reset(sys.list);

    sys.count = gp_camera_autodetect(sys.list, sys.context);
    if (sys.count < 1) {
        printf("No cameras detected.\n");
        return GP_ERROR;
    }

    printf("Number of cameras: %d\n", sys.count);
    sys.cameras = (Camera**)calloc(sizeof(Camera*), sys.count);
    for (int i = 0; i < sys.count; i++) {
        gp_list_get_name(sys.list, i, &sys.name);
        gp_list_get_value(sys.list, i, &sys.value);
        ret = open_cam(&sys.cameras[i], sys.name, sys.value, sys.context);
        if (ret < GP_OK)
            fprintf(stderr, "Camera %s on port %s failed to open\n", sys.name, sys.value);
    }
    return GP_OK;
}

export int capture_photo_to_path(Camera* cam, const char* path, GPContext* context)
{
    int fd, retval;
    CameraFile* file;
    CameraFilePath cfPath;
    strcpy(cfPath.folder, "/");
    strcpy(cfPath.name, "foo1.jpg");
    int res = gp_camera_capture(cam, GP_CAPTURE_IMAGE, &cfPath, context);
    printf("capture result: %i\n", res);
    fd = open(path, O_CREAT | O_WRONLY, 0644);
    retval = gp_file_new_from_fd(&file, fd);
    retval = gp_camera_file_get(cam, cfPath.folder, cfPath.name, GP_FILE_TYPE_NORMAL, file, context);
    retval = gp_camera_file_delete(cam, cfPath.folder, cfPath.name, context);
    gp_file_free(file);
    return res;
}
