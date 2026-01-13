#include "CameraControler.h"

void CameraControler::rotate_camera(float dx, float dy) {
    camera_->rotate_around_center(-scale_ * dy, camera_->x());
    camera_->rotate_around_center(-scale_ * dx, camera_->y());
}

void CameraControler::mouse_moved(float x, float y) {
    if (LMB_pressed_) {
        auto dx = x - x_;
        auto dy = y - y_;
        x_ = x;
        y_ = y;

        rotate_camera(dx, dy);
    }
};

void CameraControler::LMB_pressed(float x, float y) {
    LMB_pressed_ = true;
    x_ = x;
    y_ = y;
};

void CameraControler::LMB_released(float x, float y) {
    LMB_pressed_ = false;
    auto dx = x - x_;
    auto dy = y - y_;

    rotate_camera(dx, dy);
};