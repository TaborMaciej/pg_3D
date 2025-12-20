#include "Camera.h"

class CameraControler {
public:
	CameraControler() :camera_(nullptr) {
		scale_ = 0.01f;
		LMB_pressed_ = false;
	}

	CameraControler(Camera* camera) :camera_(camera) {
		scale_ = 0.01f;
		LMB_pressed_ = false;
	}

	void set_camera(Camera* camera) { camera_ = camera; }
	void rotate_camera(float dx, float dy);

	void CameraControler::mouse_moved(float x, float y);
	void LMB_pressed(float x, float y);
	void LMB_released(float x, float y);

private:
	Camera* camera_;
	float scale_;
	bool LMB_pressed_;
	float x_;
	float y_;
};