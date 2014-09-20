#pragma  once
#ifndef __YYUT_CAMERA_H__
#define __YYUT_CAMERA_H__
#include "stdafx.h"
#include "YYUT.h"
#include "YYUTMutiThread.h"
namespace YYUT
{
	class YYUTArcBall
	{
	public:
		YYUTArcBall();
		void Reset();
		void Init(HWND hwnd);
		inline void SetWindow(int width,int height,float radius=0.9f);
		inline void SetTranslationRadius(float radius_translation)
		{
			radius_tranlation_=radius_translation;
		}
		inline void SetOffset(int x,int y){offset_.x=x;offset_.y=y;}

		void OnBegin(int x,int y);
		void OnMove(int x,int y);
		void OnEnd();
		bool HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		const D3DXMATRIX* GetRotationMatrix(){return D3DXMatrixRotationQuaternion(&rotation_,&now_qua_);}
		const D3DXMATRIX* GetTranslationMatrix() const { return &translation_;}
		const D3DXMATRIX* GetTranslationDeltaMatrix() const { return &translation_delta_;}
		bool IsBeingDragged() const { return drag_;}
		D3DXQUATERNION GetQuaternionNow() const { return now_qua_;}
		void SetQuaternionNow(const D3DXQUATERNION & q){now_qua_=q;}
		static D3DXQUATERNION QuaternionFromBallPoints(const D3DXVECTOR3 &from,const D3DXVECTOR3 & to);
	protected:
		D3DXMATRIX rotation_;// orientation
		D3DXMATRIX translation_;//position
		D3DXMATRIX translation_delta_;//position
		POINT offset_;//window offset, upper_left corner window
		int width_;// window width
		int height_;//window height
		D3DXVECTOR2 center_;// center of the arc ball
		float radius_;//radius in screen coords
		float radius_tranlation_;//arc ball's radius for translating the target
		D3DXQUATERNION pre_rotate_;//quaternion before button down
		D3DXQUATERNION now_qua_;//quaternion for current drag
		bool drag_;//whether user is dragging arc ball 
		POINT last_mouse_;
		D3DXVECTOR3 down_vec_;
		D3DXVECTOR3 now_vec_;
		D3DXVECTOR3 ScreenToVector(float streen_x,float screen_y);
	};
	enum YYUT_CameraKeys
	{
		CAM_STRAFE_LEFT=0,
		CAM_STRAFE_RIGHT,
		CAM_MOVE_FORWARD,
		CAM_MOVE_BACKWARD,
		CAM_MOVE_UP,
		CAM_MOVE_DOWN,
		CAM_RESET,
		CAM_CONTROLDOWN,
		CAM_MAX_KEYS,
		CAM_UNKOWN=0xff
	};


#define KEY_WAS_DOWN_MASK 0x80
#define KEY_IS_DOWN_MASK  0x01

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08

	class YYUTBaseCamera
	{
	public:
		YYUTBaseCamera();
		virtual ~YYUTBaseCamera();
		virtual bool HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		virtual void FrameMove(float elapse_time)=0;
		virtual void Reset();
		virtual void SetViewParam(D3DXVECTOR3* eye,D3DXVECTOR3 *lookat);
		virtual void SetProjParam(float FOV,float aspect,float near_plane,float far_plane);
		virtual void SetDragRect(RECT &rc) {drag_rc_=rc;}
		void SetInvertPitch(bool invert){invert_pitch_=invert;}
		void SetHWND(HWND hwnd){ hwnd_=hwnd;}
		void SetWindow(int width,int height){width_=width;height_=height;}
		void SetDrag(bool movement_drag,float total_drag_time_to_zero=0.25f)
		{
			movement_drag_=movement_drag;
			total_drag_time_to_zero_=total_drag_time_to_zero;
		}
		void SetEnableYAxisMovement(bool enable){ enable_Y_axis_movement_=enable;}
		void SetEnablePositionMovment(bool enable){ enable_position_movement_=enable;}
		void SetClipToBoundary(bool clip_to_boundary,D3DXVECTOR3 * min_boundary,D3DXVECTOR3 *max_boundary)
		{
			clip_to_boundary_=clip_to_boundary;
			if(min_boundary)
				min_boundary_=*min_boundary;
			if(max_boundary)
				max_boundary_=*max_boundary;
		}
		void SetScalers(float rotation_scaler=0.01f,float move_scaler=5.0f){ rotation_scaler_=rotation_scaler;move_scaler_=move_scaler;}
		void SetNumberofFramesToSmoothMouseDelta(int frames){ if(frames>0) frames_to_smooth_mouse_delta_=(float)frames;}
		const D3DXMATRIX * GetViewMatrix() const { return & view_;}
		const D3DXMATRIX * GetProjMatrix() const { return & pro_;}
		const D3DXVECTOR3 * GetEyePt() const { return & eye_;}
		const D3DXVECTOR3 * GetLookatPt() const { return & lookat_;}
		float GetNearClip() const { return near_plane_;}
		float GetFarClip() const { return far_plane_;}
		bool IsBeingDragged() const { return (mouse_lbutton_down_ || mouse_rbutton_down_ ||mouse_mbutton_down_);}
		bool IsMouseLButtonDown() const { return mouse_lbutton_down_;}
		bool IsMouseMButtonDown() const { return mouse_mbutton_down_;}
		bool IsMouseRButtonDown() const { return mouse_rbutton_down_;}


	protected:
		virtual YYUT_CameraKeys MapKey(UINT key);
		bool IsKeyDown(BYTE key) const 
		{
			return (key& KEY_IS_DOWN_MASK)==KEY_IS_DOWN_MASK;
		}
		bool WasKeyDown(BYTE key) const 
		{
			return (key &KEY_WAS_DOWN_MASK)==KEY_WAS_DOWN_MASK;
		}
		void ConstrainToBoundary(D3DXVECTOR3 *);
		void UpdateMouseDelta();
		void UpdateVelocity(float elapse_time);
		void GetInput(bool get_keyboard_input,bool get_mouse_input,bool reset_cursor_after_move);
		D3DXMATRIX view_;
		D3DXMATRIX pro_;
		int keys_down_;
		byte key_mask_[CAM_MAX_KEYS];//存放各CAM的状态，主要是key控制的状态
		D3DXVECTOR3 keyboard_direction_;
		POINT last_mouse_position_;
		bool mouse_lbutton_down_;
		bool mouse_mbutton_down_;
		bool mouse_rbutton_down_;
		int current_button_mask_;//存放各种由mouse控制的state
		int mouse_wheel_delta_;
		D3DXVECTOR2 mouse_delta_;
		float frames_to_smooth_mouse_delta_;
		D3DXVECTOR3 default_eye_;
		D3DXVECTOR3 default_lookat_;
		D3DXVECTOR3 eye_;
		D3DXVECTOR3 lookat_;
		float camera_yaw_angle_;
		float camera_pitch_angle_;
		RECT drag_rc_;
		D3DXVECTOR3 velocity_;
		bool movement_drag_;
		D3DXVECTOR3 velocity_drag_;
		float drag_timer_;
		float total_drag_time_to_zero_;
		D3DXVECTOR2 rot_velocity;
		float FOV_;
		float aspect_;
		float near_plane_;
		float far_plane_;
		float rotation_scaler_;
		float move_scaler_;
		bool invert_pitch_;
		bool enable_position_movement_;
		bool enable_Y_axis_movement_;
		bool clip_to_boundary_;
		D3DXVECTOR3 min_boundary_;
		D3DXVECTOR3 max_boundary_;
		bool reset_cursor_after_move_;
		HWND hwnd_;
		int width_;
		int height_;
		YYUTMutexLock mutex_;
	};
	class YYUTEASYCamera:public YYUTBaseCamera
	{
	public:
		YYUTEASYCamera();
		~YYUTEASYCamera();
		virtual bool HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		virtual void FrameMove(float elapse_time);
		const D3DXMATRIX * GetWorldMatrix() const { return & world_;}
		void SetWindow(int width,int height,float arcball_radius=0.9f)
		{
			world_arcball_.SetWindow(width,height,arcball_radius);
			view_arcball_.SetWindow(width,height,arcball_radius);
		}
		void SetRaius(float radius){ radius_=radius;}
	protected:
		D3DXMATRIX world_;
		YYUTArcBall view_arcball_;
		YYUTArcBall world_arcball_;
		float radius_;
		D3DXMATRIX last_world_rotate;
		D3DXMATRIX final_world_rotate;
	};
	class YYUTModelViewerCamera:public YYUTBaseCamera
	{
	public:
		YYUTModelViewerCamera();
		~YYUTModelViewerCamera();
		virtual bool HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		virtual void FrameMove(float elapse_time);
		virtual void Reset();
		virtual void SetViewParam(D3DXVECTOR3* eye,D3DXVECTOR3 *lookat);
		virtual void SetDragRect(RECT &rc);
		void SetButtonMasks(int rotate_model_button_mask=MOUSE_LEFT_BUTTON,
			int zoom_button_mask=MOUSE_WHEEL)
		{
			rotate_model_button_mask_=rotate_model_button_mask;
			zoom_button_mask_=zoom_button_mask;
		}
		void SetAttachCameraTomodel(bool enable=false){ attach_camera_to_model_=enable;}
		void SetWindow(int width,int height,float arcball_radius=0.9f)
		{
			YYUTBaseCamera::SetWindow(width,height);
			world_arcball_.SetWindow(width,height,arcball_radius);
			view_arcball_.SetWindow(width,height,arcball_radius);
		}
		void SetRadius(float default_radius=15.0f,float min_radius=1.0f,float max_radius=FLT_MAX)
		{
			default_radius_=default_radius;
			min_radius_=min_radius;
			max_radius_=max_radius;
		}
		void SetModelCenter(D3DXVECTOR3 model_center){ model_center_=model_center;}
		void SetLimitPitch(bool limit){ limit_pitch_=limit;}
		void SetViewQuat(D3DXQUATERNION q){ view_arcball_.SetQuaternionNow(q);drag_since_last_update_=true;}
		void SetWorldQuat(D3DXQUATERNION q){world_arcball_.SetQuaternionNow(q);drag_since_last_update_=true;}
		const D3DXMATRIX * GetWorldMatrix() const { return & world_;}
		void SetWorldMatrix(D3DXMATRIX & world) {world_=world;drag_since_last_update_=true; }
	protected:
		YYUTArcBall world_arcball_;
		YYUTArcBall view_arcball_;
		D3DXVECTOR3 model_center_;
		D3DXMATRIX model_last_rot_;
		D3DXMATRIX camera_rot_last_;
		D3DXMATRIX model_rot_;
		D3DXMATRIX world_;
		int rotate_model_button_mask_;
		int zoom_button_mask_;
		int rotate_camera_button_mask_;
		bool attach_camera_to_model_;
		bool limit_pitch_;
		float radius_;
		float default_radius_;
		float min_radius_;
		float max_radius_;
		bool drag_since_last_update_;
	};
	class YYUTFirstPersonCamera:public YYUTBaseCamera
	{
	public:
		YYUTFirstPersonCamera();
		virtual ~YYUTFirstPersonCamera();
		virtual void FrameMove(float elapse_time);
		D3DXMATRIX* GetWroldMatrix() { return & camera_world_;}
		const D3DXVECTOR3 * GetEye() { return (D3DXVECTOR3*)&camera_world_._41;}
		const D3DXVECTOR3 * GetWorldRight() const { return (D3DXVECTOR3*)&camera_world_._11;}
		const D3DXVECTOR3 * GetWorldUp() const { return (D3DXVECTOR3*)&camera_world_._21;}
		const D3DXVECTOR3 * GetWorldAhead() const { return (D3DXVECTOR3*)&camera_world_._31;}
	protected:
		D3DXMATRIX camera_world_;
	};
}
#endif

