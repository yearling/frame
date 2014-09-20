#include "stdafx.h"
#include "YYUTMutiScreen.h"
#include "YYUTCamera.h"
#include <xutility>
#if defined( DEBUG ) || defined( _DEBUG )
#include <iostream>
using std::cout;
using std::endl;
#endif
namespace YYUT
{

	void YYUTArcBall::Reset()
	{
		D3DXQuaternionIdentity(&pre_rotate_);
		D3DXQuaternionIdentity(&now_qua_);
		D3DXMatrixIdentity(&rotation_);
		D3DXMatrixIdentity(&translation_);
		D3DXMatrixIdentity(&translation_delta_);
		drag_=false;
		radius_=1.0f;
		radius_tranlation_=1.0f;
	}

	YYUTArcBall::YYUTArcBall()
	{
		Reset();
		down_vec_=D3DXVECTOR3(0,0,0);
		now_vec_=D3DXVECTOR3(0,0,0);
		offset_.x=offset_.y=0;
	}

	void YYUTArcBall::Init(HWND hwnd)
	{
		RECT rc;
		GetClientRect(hwnd,&rc);
		SetWindow(rc.right,rc.bottom);
	}

	void YYUTArcBall::SetWindow(int width,int height,float radius/*=0.9f*/)
	{
		width_=width;
		height_=height;
		radius_=radius;
		center_=D3DXVECTOR2(width_/2.0f,height_/2.0f);
	}

	D3DXQUATERNION YYUTArcBall::QuaternionFromBallPoints(const D3DXVECTOR3 &from,const D3DXVECTOR3 & to)
	{
		D3DXVECTOR3 part;
		//from 和 to 都是单位向量，所以点乘的结果就是cos⁡θ
		float fdot=D3DXVec3Dot(&from,&to);
		//from 和 to 都是单位向量，所点叉乘的结果可能不是单位向量，模为|A||B|*sin⁡θ
		//所以A/sin⁡θ才得到单位向量，然后四元数的向量部分是A*sin⁡θ，所以就是quaternion(AXB,A.B)的结果了。
		D3DXVec3Cross(&part,&from,&to);
		return D3DXQUATERNION(part.x,part.y,part.z,fdot);
	}

	
	
	D3DXVECTOR3 YYUTArcBall::ScreenToVector(float screen_x,float screen_y)
	{
		//设为左手坐标系，前Z正，右X正，上Y正，这样子我们操作的是负Z半轴的那个球
		float x=(screen_x-offset_.x-width_/2)/(radius_*width_/2);
		float y=-(screen_y-offset_.y-height_/2)/(radius_*height_/2);
		float z=0.0f;
		float mag=x*x+y*y;
		if(mag>1.0f)
		{
			float scale=1.0f/sqrtf(mag);
			x*=scale;
			y*=scale;
		}
		else
			z=-sqrtf(1.0f-mag);
		return D3DXVECTOR3(x,y,z);
	}
	void YYUTArcBall::OnBegin(int x,int y)
	{
		if(x>=offset_.x && x<=offset_.x+width_ && y>=offset_.y && y<=offset_.y+height_)
		{
			drag_=true;
			//down_qua_用来记录上次旋转的结果
			pre_rotate_=now_qua_;
			//down_vec_靠这个来标识起点
			down_vec_=ScreenToVector((float)x,(float)y);
		}
	}
	void YYUTArcBall::OnMove(int x,int y)
	{
		if(drag_)
		{
			now_vec_=ScreenToVector((float)x,(float)y);
			now_qua_=pre_rotate_*QuaternionFromBallPoints(down_vec_,now_vec_);
		}
	}

	void YYUTArcBall::OnEnd()
	{
		drag_=false;
	}

	bool YYUTArcBall::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		int mouse_x=(short)LOWORD(lParam);
		int mouse_y=(short)HIWORD(lParam);
		switch(uMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			SetCapture(hwnd);
			OnBegin(mouse_x,mouse_y);
			return true;
		case WM_LBUTTONUP:
			ReleaseCapture();
			OnEnd();
			return true;
		case WM_CAPTURECHANGED:
			if((HWND)lParam!=hwnd)
			{
				ReleaseCapture();
				OnEnd();
			}
			return true;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
			SetCapture(hwnd);
			last_mouse_.x=mouse_x;
			last_mouse_.y=mouse_y;
			return true;
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			ReleaseCapture();
			return true;
		case WM_MOUSEMOVE:
			if(MK_LBUTTON &wParam)
			{
				OnMove(mouse_x,mouse_y);
			}
			else if((MK_RBUTTON & wParam )||(MK_MBUTTON & wParam))
			{
				float delta_x=(last_mouse_.x-mouse_x)*radius_tranlation_/width_;
				float delta_y=(last_mouse_.y-mouse_y)*radius_tranlation_/height_;
				if(wParam & MK_RBUTTON)
				{
					D3DXMatrixTranslation(&translation_delta_,-2*delta_x,2*delta_y,0.0f);
					D3DXMatrixMultiply(&translation_,&translation_,&translation_delta_);
				}
				else
				{
					D3DXMatrixTranslation(&translation_delta_,0.0f,0.0f,5*delta_y);
					D3DXMatrixMultiply(&translation_,&translation_,&translation_delta_);
				}
			}
			return true;
		}
		return false;
	}

	YYUTBaseCamera::YYUTBaseCamera()
	{
		keys_down_=0;
		ZeroMemory(key_mask_,sizeof(byte)*CAM_MAX_KEYS);
		D3DXVECTOR3 eye=D3DXVECTOR3(0.0f,0.0f,0.0f);
		D3DXVECTOR3 lookat=D3DXVECTOR3(0.0f,0.0f,0.0f);
		SetViewParam(&eye,&lookat);
		SetProjParam(D3DX_PI/4,1.0f,1.0f,1000.f);
		GetCursorPos(&last_mouse_position_);
		mouse_lbutton_down_=false;
		mouse_mbutton_down_=false;
		mouse_rbutton_down_=false;
		current_button_mask_=0;
		mouse_wheel_delta_=0;
		camera_yaw_angle_=0.0f;
		camera_pitch_angle_=0.0f;
		SetRect(&drag_rc_,LONG_MIN,LONG_MIN,LONG_MAX,LONG_MAX);
		velocity_=D3DXVECTOR3(0.0f,0.0f,0.0f);
		movement_drag_=false;
		velocity_drag_=D3DXVECTOR3(0,0,0);
		drag_timer_=0.0f;
		total_drag_time_to_zero_=0.25;
		rot_velocity=D3DXVECTOR2(0,0);
		rotation_scaler_=0.01f;
		move_scaler_=5.0f;
		invert_pitch_=false;
		enable_position_movement_=true;
		enable_Y_axis_movement_=true;
		mouse_delta_=D3DXVECTOR2(0,0);
		frames_to_smooth_mouse_delta_=2.0f;
		clip_to_boundary_=false;
		min_boundary_=D3DXVECTOR3(-1,-1,-1);
		max_boundary_=D3DXVECTOR3(1,1,1);
		reset_cursor_after_move_=false;
		hwnd_=NULL;
	}

	void YYUTBaseCamera::SetViewParam(D3DXVECTOR3* eye,D3DXVECTOR3 *lookat)
	{
		YYUTMutexLockGuard lock(mutex_);
		if(NULL==eye || NULL==lookat)
			return;
		default_eye_=eye_=*eye;
		default_lookat_=lookat_= *lookat;
		D3DXVECTOR3 up(0,1,0);
		D3DXMatrixLookAtLH(&view_,&eye_,&lookat_,&up);
		//先pitch 再yaw
		//得到的view其实是视坐标基的逆，所以要先进行一次逆得到视坐标的基
		//然后得到Z轴坐标，进行计算。
		//因为是先pitch再yaw，也就是说，先绕x轴，再绕y轴把z轴从世界坐标绕到现在的视坐标
		//旋转保持轴长度不变和向量到旋转轴的距离不变。
		//1.绕X轴把Z轴转到YZ平面
		//2.绕Y轴把Z轴转到位置
		//如果要计算的话要反过来算,先算2
		//就是Z轴现在的位置绕Y轴转到YZ平面上，投影到XZ平面上，角度为-arctan(x/z),反过来也就是2，就是arctan(x/z)
		//然后因为刚才绕Y轴旋转，投影到XZ平面上的sqrt(x^2+z^2)长度不变，就是现在Y轴投到Z轴上的长度，
		//所以绕X轴的旋角为arctan(y,sqrt(x^2+z^2)),角度从Z到Y，反方向，取负
		D3DXMATRIX inverse_view;
		D3DXMatrixInverse(&inverse_view,NULL,&view_);
		D3DXVECTOR3 * z_basis=(D3DXVECTOR3*)&inverse_view._31;
		camera_yaw_angle_=atan2f(z_basis->x,z_basis->z);
		float len=sqrtf(z_basis->z*z_basis->z+z_basis->x*z_basis->x);
		camera_pitch_angle_=-atan2f(z_basis->y,len);
	}

	void YYUTBaseCamera::SetProjParam(float FOV,float aspect,float near_plane,float far_plane)
	{
		YYUTMutexLockGuard lock(mutex_);
		FOV_=FOV;
		aspect_=aspect;
		near_plane_=near_plane;
		far_plane_=far_plane;
		D3DXMatrixPerspectiveFovLH(&pro_,FOV_,aspect_,near_plane_,far_plane_);
	}
	//通过接收windos message来设置某些状态位
	bool YYUTBaseCamera::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		YYUTMutexLockGuard lock(mutex_);
		switch(uMsg)
		{
		case WM_KEYDOWN:
			{
				YYUT_CameraKeys maped_key=MapKey((UINT)wParam);
				if(maped_key!=CAM_UNKOWN)
				{
					if(false==IsKeyDown(key_mask_[maped_key]))
					{
						key_mask_[maped_key]=KEY_WAS_DOWN_MASK | KEY_IS_DOWN_MASK;
						++keys_down_;
					}
				}
				break;
			}
		case WM_KEYUP:
			{
				YYUT_CameraKeys mapped_key=MapKey((UINT)wParam);
				if(mapped_key!=CAM_UNKOWN && (DWORD)mapped_key <8)
				{
					key_mask_[mapped_key]&=~KEY_IS_DOWN_MASK;
					--keys_down_;
				}
				break;
			}
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
			{
				POINT cursor_pos={(short)LOWORD(lParam),(short)HIWORD(lParam)};
				if((uMsg==WM_LBUTTONDOWN ||uMsg==WM_LBUTTONDBLCLK ) && PtInRect(&drag_rc_,cursor_pos) )
				{
					mouse_lbutton_down_=true;
					current_button_mask_|=MOUSE_LEFT_BUTTON;
				}

				if((uMsg==WM_MBUTTONDOWN ||uMsg==WM_MBUTTONDBLCLK ) && PtInRect(&drag_rc_,cursor_pos) )
				{
					mouse_mbutton_down_=true;
					current_button_mask_|=MOUSE_MIDDLE_BUTTON;
				}
				if((uMsg==WM_RBUTTONDOWN ||uMsg==WM_RBUTTONDBLCLK ) && PtInRect(&drag_rc_,cursor_pos) )
				{
					mouse_rbutton_down_=true;
					current_button_mask_|=MOUSE_RIGHT_BUTTON;
				}
				SetCapture(hwnd);
				GetCursorPos(&last_mouse_position_);
				return true;
			}
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			{
				if(uMsg==WM_LBUTTONUP)
				{
					mouse_lbutton_down_=false;
					current_button_mask_&=~MOUSE_LEFT_BUTTON;
				}
				if(uMsg==WM_MBUTTONUP)
				{
					mouse_mbutton_down_=false;
					current_button_mask_&=~MOUSE_MIDDLE_BUTTON;
				}
				if(uMsg==WM_RBUTTONUP)
				{
					mouse_rbutton_down_=false;
					current_button_mask_&=~MOUSE_RIGHT_BUTTON;
				}
				if(!mouse_lbutton_down_ && !mouse_mbutton_down_ && !mouse_rbutton_down_)
				{
					ReleaseCapture();
				}
				break;
			}
		case WM_CAPTURECHANGED:
			{
				if((HWND)lParam !=hwnd)
				{
					if((current_button_mask_ & MOUSE_LEFT_BUTTON )||
						(current_button_mask_ & MOUSE_MIDDLE_BUTTON)||
						(current_button_mask_ & MOUSE_RIGHT_BUTTON))
					{
						mouse_lbutton_down_=false;
						mouse_mbutton_down_=false;
						mouse_rbutton_down_=false;
						current_button_mask_ &=~MOUSE_LEFT_BUTTON;
						current_button_mask_ &=~MOUSE_RIGHT_BUTTON;
						current_button_mask_ &=~MOUSE_MIDDLE_BUTTON;
						ReleaseCapture();
					}
				}
				break;
			}
		case WM_MOUSEWHEEL:
			mouse_wheel_delta_+=(short)HIWORD(wParam);
			break;
		}
		return false;
	}
	//把按键映射成CAM的虚拟码
	YYUT_CameraKeys YYUTBaseCamera::MapKey(UINT key)
	{
		switch( key )
		{
		case VK_CONTROL:
			return CAM_CONTROLDOWN;
		case VK_LEFT:
			return CAM_STRAFE_LEFT;
		case VK_RIGHT:
			return CAM_STRAFE_RIGHT;
		case VK_UP:
			return CAM_MOVE_FORWARD;
		case VK_DOWN:
			return CAM_MOVE_BACKWARD;
		case VK_PRIOR:
			return CAM_MOVE_UP;        // pgup
		case VK_NEXT:
			return CAM_MOVE_DOWN;      // pgdn

		case 'A':
			return CAM_STRAFE_LEFT;
		case 'D':
			return CAM_STRAFE_RIGHT;
		case 'W':
			return CAM_MOVE_FORWARD;
		case 'S':
			return CAM_MOVE_BACKWARD;
		case 'Q':
			return CAM_MOVE_DOWN;
		case 'E':
			return CAM_MOVE_UP;

		case VK_NUMPAD4:
			return CAM_STRAFE_LEFT;
		case VK_NUMPAD6:
			return CAM_STRAFE_RIGHT;
		case VK_NUMPAD8:
			return CAM_MOVE_FORWARD;
		case VK_NUMPAD2:
			return CAM_MOVE_BACKWARD;
		case VK_NUMPAD9:
			return CAM_MOVE_UP;
		case VK_NUMPAD3:
			return CAM_MOVE_DOWN;

		case VK_HOME:
			return CAM_RESET;
		}

		return CAM_UNKOWN;
	}

	void YYUTBaseCamera::GetInput(bool get_keyboard_input,bool get_mouse_input,bool reset_cursor_after_move)
	{
		reset_cursor_after_move_=reset_cursor_after_move;
		keyboard_direction_=D3DXVECTOR3(0,0,0);
		if(get_keyboard_input)
		{
			if(IsKeyDown(key_mask_[CAM_MOVE_FORWARD]))
				keyboard_direction_.z+=1.0f;
			if(IsKeyDown(key_mask_[CAM_MOVE_BACKWARD]))
				keyboard_direction_.z-=1.0f;
			if(enable_Y_axis_movement_)
			{
				if(IsKeyDown(key_mask_[CAM_MOVE_UP]))
					keyboard_direction_.y+=1.0f;
				if(IsKeyDown(key_mask_[CAM_MOVE_DOWN]))
					keyboard_direction_.y-=1.0f;
			}
			if(IsKeyDown(key_mask_[CAM_STRAFE_RIGHT]))
				keyboard_direction_.x+=1.0f;
			if(IsKeyDown(key_mask_[CAM_STRAFE_LEFT]))
				keyboard_direction_.x-=1.0f;
		}
		if(get_mouse_input)
		{
			UpdateMouseDelta();
		}
	}

	void YYUTBaseCamera::UpdateMouseDelta()
	{
		POINT cur_mouse_delta;
		POINT cur_mouse_pos;
		GetCursorPos(&cur_mouse_pos);
		cur_mouse_delta.x=cur_mouse_pos.x-last_mouse_position_.x;
		cur_mouse_delta.y=cur_mouse_pos.y-last_mouse_position_.y;
		last_mouse_position_=cur_mouse_pos;
		if(reset_cursor_after_move_)
		{
			POINT center;
			center.x=(width_)/2;
			center.y=(height_)/2;
			::ClientToScreen(hwnd_,&center);
			SetCursorPos(center.x,center.y);
			last_mouse_position_=center;
		}
		float percent_new=1.0f/frames_to_smooth_mouse_delta_;
		float percent_old=1.0f-percent_new;
		mouse_delta_.x=mouse_delta_.x*percent_old + cur_mouse_delta.x*percent_new;
		mouse_delta_.y=mouse_delta_.y*percent_old + cur_mouse_delta.y*percent_new;
		rot_velocity=mouse_delta_*rotation_scaler_;
	}

	void YYUTBaseCamera::UpdateVelocity(float elapse_time)
	{
		D3DXMATRIX rot_delta;
		rot_velocity=mouse_delta_*rotation_scaler_;
		D3DXVECTOR3 accel=keyboard_direction_;
		D3DXVec3Normalize(&accel,&accel);
		accel*=move_scaler_;
		if(movement_drag_)
		{
			if(D3DXVec3LengthSq(&accel)>0)
			{
				velocity_=accel;
				drag_timer_=total_drag_time_to_zero_;
				velocity_drag_=accel/drag_timer_;//加速度
			}
			else
			{
				if(drag_timer_>0)
				{
					velocity_-=velocity_drag_*elapse_time;
					drag_timer_-=elapse_time;
				}
				else
					velocity_=D3DXVECTOR3(0,0,0);
			}
		}
		else
			velocity_=accel;
	}

	void YYUTBaseCamera::ConstrainToBoundary(D3DXVECTOR3 *v)
	{
		v->x=(std::max)(v->x,min_boundary_.x);
		v->y=(std::max)(v->y,min_boundary_.y);
		v->z=(std::max)(v->z,min_boundary_.z);
		v->x=(std::min)(v->x,max_boundary_.x);
		v->y=(std::min)(v->y,max_boundary_.y);
		v->z=(std::min)(v->z,max_boundary_.z);

	}

	void YYUTBaseCamera::Reset()
	{
		YYUTMutexLockGuard lock(mutex_);
		SetViewParam(&default_eye_,&default_lookat_);
	}

	YYUTBaseCamera::~YYUTBaseCamera()
	{

	}


	YYUTModelViewerCamera::YYUTModelViewerCamera()
	{
		D3DXMatrixIdentity(&world_);
		D3DXMatrixIdentity(&model_rot_);
		D3DXMatrixIdentity(&model_last_rot_);
		D3DXMatrixIdentity(&camera_rot_last_);
		model_center_=D3DXVECTOR3(0,0,0);
		default_radius_=15.0f;
		min_radius_=1.0f;
		max_radius_=FLT_MAX;
		limit_pitch_=false;
		enable_position_movement_=false;
		attach_camera_to_model_=false;
		rotate_model_button_mask_=MOUSE_LEFT_BUTTON;
		zoom_button_mask_=MOUSE_WHEEL;
		rotate_camera_button_mask_=MOUSE_RIGHT_BUTTON;
		drag_since_last_update_=true;
		radius_=35.0f;

	}

	YYUTModelViewerCamera::~YYUTModelViewerCamera()
	{

	}

	void YYUTModelViewerCamera::FrameMove(float elapse_time)
	{
		YYUTMutexLockGuard lock(mutex_);
		if(IsKeyDown(key_mask_[CAM_RESET]))
			Reset();
		if(!drag_since_last_update_ && 0==keys_down_)
			return;
		drag_since_last_update_=false;
		GetInput(enable_position_movement_,current_button_mask_!=0,false);
		UpdateVelocity(elapse_time);
		D3DXVECTOR3 pos_delta=velocity_*elapse_time;
		if(mouse_wheel_delta_ && zoom_button_mask_==MOUSE_WHEEL)
		{
			radius_-=(float)(mouse_wheel_delta_*radius_*0.1/120.0f);
		}
		radius_=(std::min)(max_radius_,radius_);
		radius_=(std::max)(min_radius_,radius_);
		mouse_wheel_delta_=0;
		D3DXMATRIX camera_rot;
		//把摄像头移动的方向取反，这样子看着比较舒服，操作正方向，不是把基变，还是基坐标
		D3DXMatrixInverse(&camera_rot,NULL,view_arcball_.GetRotationMatrix());
		D3DXVECTOR3 world_up,world_ahead;
		D3DXVECTOR3 local_up=D3DXVECTOR3(0,1,0);
		D3DXVECTOR3 local_ahead=D3DXVECTOR3(0,0,1);
		D3DXVec3TransformCoord(&world_up,&local_up,&camera_rot);
		D3DXVec3TransformCoord(&world_ahead,&local_ahead,&camera_rot);
		//变化是在摄像头空间里变化的，变为world空间
		//D3DXVECTOR3 pos_delta_world;
		//D3DXVec3TransformCoord( &pos_delta_world,&pos_delta,&camera_rot);
		//lookat=物体位置+之前位置
		/*lookat_+=pos_delta_world;
		if(clip_to_boundary_)
			ConstrainToBoundary(&lookat_);*/
		eye_=lookat_-world_ahead*radius_;
		D3DXMatrixLookAtLH(&view_,&eye_,&lookat_,&world_up);
		//视变换坐标取逆得到视坐标的基，并且至于原点处
		D3DXMATRIX invert_view;
		D3DXMatrixInverse(&invert_view,NULL,&view_);
		//invert_view._41=invert_view._42=invert_view._43=0;
		D3DXMATRIX invert_model_last_rot;
		D3DXMatrixInverse(&invert_model_last_rot,NULL,&model_last_rot_);

		D3DXMATRIX model_rot;
		model_rot=*world_arcball_.GetRotationMatrix();
		model_rot_*=view_*invert_model_last_rot*model_rot*invert_view;
		//这个时候也不知道旋转到哪了，反正最后放到lookat的位置就行了
		if(view_arcball_.IsBeingDragged() && attach_camera_to_model_ && !IsKeyDown(key_mask_[CAM_CONTROLDOWN]))
		{
			D3DXMATRIX invert_camera_last_rot;
			D3DXMatrixInverse(&invert_camera_last_rot,NULL,&camera_rot_last_);
			D3DXMATRIX camera_rot_delta=invert_camera_last_rot*camera_rot;
			model_rot_*=camera_rot_delta;
		}
		camera_rot_last_=camera_rot;
		model_last_rot_=model_rot;
		
		D3DXVECTOR3 *x_basis=(D3DXVECTOR3*) &model_rot_._11;
		D3DXVECTOR3 *y_basis=(D3DXVECTOR3*) &model_rot_._21;
		D3DXVECTOR3 *z_basis=(D3DXVECTOR3*) &model_rot_._31;
		D3DXVec3Normalize(x_basis,x_basis);
		D3DXVec3Cross(y_basis,z_basis,x_basis);
		D3DXVec3Normalize(y_basis,y_basis);
		D3DXVec3Cross(z_basis,x_basis,y_basis);

		model_rot_._41=lookat_.x;
		model_rot_._42=lookat_.y;
		model_rot_._43=lookat_.z;
		D3DXMATRIX trans;
		D3DXMatrixTranslation(&trans,-model_center_.x,-model_center_.y,-model_center_.z);
		world_=trans*model_rot_;
	}

	void YYUTModelViewerCamera::SetDragRect(RECT &rc)
	{
		YYUTBaseCamera::SetDragRect(rc);
		world_arcball_.SetOffset(rc.left,rc.top);
		view_arcball_.SetOffset(rc.left,rc.top);
		SetWindow(rc.right-rc.left,rc.bottom-rc.top);
	}

	void YYUTModelViewerCamera::Reset()
	{
		YYUTBaseCamera::Reset();
		D3DXMatrixIdentity(&world_);
		D3DXMatrixIdentity(&model_rot_);
		D3DXMatrixIdentity(&model_last_rot_);
		D3DXMatrixIdentity(&camera_rot_last_);
		radius_=default_radius_;
		world_arcball_.Reset();
		view_arcball_.Reset();
	}

	void YYUTModelViewerCamera::SetViewParam(D3DXVECTOR3* eye,D3DXVECTOR3 *lookat)
	{
		YYUTBaseCamera::SetViewParam(eye,lookat);
		D3DXQUATERNION quat;
		D3DXMATRIX rotation;
		D3DXVECTOR3 up(0,1,0);
		D3DXMatrixLookAtLH(&rotation,eye,lookat,&up);
		D3DXQuaternionRotationMatrix(&quat,&rotation);
		view_arcball_.SetQuaternionNow(quat);
		D3DXVECTOR3 eye_to_point;
		D3DXVec3Subtract(&eye_to_point,lookat,eye);
		SetRadius(D3DXVec3Length(&eye_to_point));
		drag_since_last_update_=true;
	}

	bool YYUTModelViewerCamera::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		YYUTBaseCamera::HandleMessage(hwnd,uMsg,wParam,lParam);
		if(((uMsg==WM_LBUTTONDOWN || uMsg==WM_LBUTTONDBLCLK) && rotate_model_button_mask_ & MOUSE_LEFT_BUTTON)||
			((uMsg==WM_MBUTTONDOWN || uMsg==WM_MBUTTONDBLCLK) && rotate_model_button_mask_ & MOUSE_MIDDLE_BUTTON)||
			((uMsg==WM_RBUTTONDOWN || uMsg==WM_RBUTTONDBLCLK) && rotate_model_button_mask_ & MOUSE_RIGHT_BUTTON))
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			world_arcball_.OnBegin(x,y);
		}
		if(((uMsg==WM_LBUTTONDOWN || uMsg==WM_LBUTTONDBLCLK) && rotate_camera_button_mask_ & MOUSE_LEFT_BUTTON)||
			((uMsg==WM_MBUTTONDOWN || uMsg==WM_MBUTTONDBLCLK) && rotate_camera_button_mask_ & MOUSE_MIDDLE_BUTTON)||
			((uMsg==WM_RBUTTONDOWN || uMsg==WM_RBUTTONDBLCLK) && rotate_camera_button_mask_ & MOUSE_RIGHT_BUTTON))
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			view_arcball_.OnBegin(x,y);
		}
		if(uMsg==WM_MOUSEMOVE)
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			world_arcball_.OnMove(x,y);
			view_arcball_.OnMove(x,y);
		}
		if(((uMsg==WM_LBUTTONUP) && rotate_model_button_mask_ & MOUSE_LEFT_BUTTON)||
			((uMsg==WM_MBUTTONUP) && rotate_model_button_mask_ & MOUSE_MIDDLE_BUTTON)||
			((uMsg==WM_RBUTTONUP ) && rotate_model_button_mask_ & MOUSE_RIGHT_BUTTON))
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			world_arcball_.OnEnd();
		}
		if(((uMsg==WM_LBUTTONUP) && rotate_camera_button_mask_ & MOUSE_LEFT_BUTTON)||
			((uMsg==WM_MBUTTONUP) && rotate_camera_button_mask_ & MOUSE_MIDDLE_BUTTON)||
			((uMsg==WM_RBUTTONUP ) && rotate_camera_button_mask_ & MOUSE_RIGHT_BUTTON))
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			view_arcball_.OnEnd();
		}
		if(uMsg==WM_CAPTURECHANGED)
		{
			if(hwnd!=(HWND)lParam)
			{
				if((rotate_model_button_mask_& MOUSE_LEFT_BUTTON) ||
					(rotate_model_button_mask_& MOUSE_MIDDLE_BUTTON) ||
					(rotate_model_button_mask_ & MOUSE_RIGHT_BUTTON))
				{
					world_arcball_.OnEnd();
				}
				if((rotate_camera_button_mask_& MOUSE_LEFT_BUTTON) ||
					(rotate_camera_button_mask_& MOUSE_MIDDLE_BUTTON) ||
					(rotate_camera_button_mask_ & MOUSE_RIGHT_BUTTON))
				{
					view_arcball_.OnEnd();
				}

			}
		}
		if( uMsg == WM_LBUTTONDOWN ||
			uMsg == WM_LBUTTONDBLCLK ||
			uMsg == WM_MBUTTONDOWN ||
			uMsg == WM_MBUTTONDBLCLK ||
			uMsg == WM_RBUTTONDOWN ||
			uMsg == WM_RBUTTONDBLCLK ||
			uMsg == WM_LBUTTONUP ||
			uMsg == WM_MBUTTONUP ||
			uMsg == WM_RBUTTONUP ||
			uMsg == WM_MOUSEWHEEL ||
			uMsg == WM_MOUSEMOVE )
		{
			drag_since_last_update_ = true;
		}
		return false;
		
	}



	YYUTEASYCamera::YYUTEASYCamera()
	{
		radius_=10.0f;
		D3DXMatrixIdentity(&world_);
		D3DXMatrixIdentity(&last_world_rotate);
		D3DXMatrixIdentity(&final_world_rotate);
	}

	bool YYUTEASYCamera::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		YYUTBaseCamera::HandleMessage(hwnd,uMsg,wParam,lParam);
		if(uMsg==WM_LBUTTONDOWN )
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			world_arcball_.OnBegin(x,y);
		}
		if(uMsg==WM_RBUTTONDOWN )
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			view_arcball_.OnBegin(x,y);
		} 
		if(uMsg==WM_MOUSEMOVE)
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			world_arcball_.OnMove(x,y);
			view_arcball_.OnMove(x,y);
		}
		if(uMsg==WM_LBUTTONUP)
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			world_arcball_.OnEnd();
		}
		if(uMsg==WM_RBUTTONUP)
		{
			int x=(short)LOWORD(lParam);
			int y=(short)HIWORD(lParam);
			view_arcball_.OnEnd();
		}



		if(uMsg==WM_CAPTURECHANGED)
		{
			if(hwnd!=(HWND)lParam)
			{
					world_arcball_.OnEnd();
			}
		}
		return false;
	}

	void YYUTEASYCamera::FrameMove(float elapse_time)
	{
		D3DXMATRIX view_arcball;
		D3DXMatrixInverse(&view_arcball,nullptr,view_arcball_.GetRotationMatrix());
		D3DXVECTOR3 local_ahead(0.0f,0.0f,1.0f);
		D3DXVECTOR3 local_up(0.0f,1.0f,0.0f);
		D3DXVECTOR3 world_lookat,world_up;
		D3DXVec3TransformCoord(&world_up,&local_up,&view_arcball);
		D3DXVec3TransformCoord(&world_lookat,&local_ahead,&view_arcball);
		eye_=lookat_-world_lookat*radius_;
		D3DXMatrixLookAtLH(&view_,&eye_,&lookat_,&world_up);
		D3DXMATRIX view_inverse;
		D3DXMATRIX world_rotate_view=*world_arcball_.GetRotationMatrix();
		D3DXMATRIX last_world_rotate_invert;
		D3DXMatrixInverse(&last_world_rotate_invert,nullptr,&last_world_rotate);
		D3DXMatrixInverse(&view_inverse,nullptr,&view_);
		final_world_rotate*=view_*last_world_rotate_invert*world_rotate_view*view_inverse;
		//这个时候也不知道旋转哪去了，who care,最后把那放到指定位置就行了。
		world_=final_world_rotate;
		world_._41=lookat_.x;
		world_._42=lookat_.y;
		world_._43=lookat_.z;
		last_world_rotate=world_rotate_view;
	}

	YYUTEASYCamera::~YYUTEASYCamera()
	{

	}


	YYUTFirstPersonCamera::YYUTFirstPersonCamera()
	{
		enable_position_movement_=true;
		reset_cursor_after_move_=true;
		ShowCursor(false);
	}

	YYUTFirstPersonCamera::~YYUTFirstPersonCamera()
	{

	}

	void YYUTFirstPersonCamera::FrameMove(float elapse_time)
	{
		YYUTMutexLockGuard lock(mutex_);
		if(IsKeyDown(key_mask_[CAM_RESET]))
			Reset();
		GetInput(enable_position_movement_,true,reset_cursor_after_move_);
		UpdateVelocity(elapse_time);
		D3DXVECTOR3 pos_delta=velocity_*elapse_time;
		float yaw_delta=rot_velocity.x;
		float pitch_delta=rot_velocity.y;
		camera_yaw_angle_+=yaw_delta;
		camera_pitch_angle_+=pitch_delta;
		//上下伏仰限制在+-90度
		camera_pitch_angle_=(std::max)(-D3DX_PI/2,camera_pitch_angle_);
		camera_pitch_angle_=(std::min)(D3DX_PI/2,camera_pitch_angle_);
		D3DXMATRIX camera_rot;
		D3DXMatrixRotationYawPitchRoll(&camera_rot,camera_yaw_angle_,camera_pitch_angle_,0);
		D3DXVECTOR3 world_up,world_ahead;
		D3DXVECTOR3 local_up(0,1,0);
		D3DXVECTOR3 local_ahead(0,0,1);
		D3DXVec3TransformCoord(&world_up,&local_up,&camera_rot);
		D3DXVec3TransformCoord(&world_ahead,&local_ahead,&camera_rot);
		D3DXVECTOR3 world_pos_delta;
		D3DXVec3TransformCoord(&world_pos_delta,&pos_delta,&camera_rot);
		eye_+=world_pos_delta;
		lookat_=eye_+world_ahead;
		D3DXMatrixLookAtLH(&view_,&eye_,&lookat_,&world_up);
		D3DXMatrixInverse(&camera_world_,nullptr,&view_);
	}

}
