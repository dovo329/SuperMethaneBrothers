/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 * Program WebSite: http://methane.sourceforge.net/index.html              *
 *                                                                         *
 ***************************************************************************/

//------------------------------------------------------------------------------
// Methane brothers main source file
//------------------------------------------------------------------------------
#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>
#include <ClanLib/swrender.h>
#include <ClanLib/gl.h>
#include <ClanLib/gl1.h>
#include <ClanLib/sound.h>
#include <ClanLib/mikmod.h>

#include "global.h"

#include "doc.h"

RenderTarget GLOBAL_RenderTarget = opengl2;
bool GLOBAL_SoundEnable = true;

//------------------------------------------------------------------------------
// Keyboard stuff
//------------------------------------------------------------------------------
static int LastKey = 0;
//#define WINDOW_RESIZE_DEBUG

static CL_DisplayWindow* m_WindowPtr;
// Joystick stuff
//#define JOYSTICK_CONSOLE_DEBUG

#define JOYSTICK_X_AXIS_ID 0
#define JOYSTICK_Y_AXIS_ID 1
#define JOYSTICK_FIRE_BUTTON_ID 2
#define JOYSTICK_JUMP_BUTTON_ID 1
#define JOYSTICK_SELECT_BUTTON_ID 8
#define JOYSTICK_START_BUTTON_ID 9

#define JOYSTICK_STATE_LEFT_INDEX 0
#define JOYSTICK_STATE_RIGHT_INDEX 1
#define JOYSTICK_STATE_UP_INDEX 2
#define JOYSTICK_STATE_DOWN_INDEX 3
#define JOYSTICK_STATE_FIRE_INDEX 4
#define JOYSTICK_STATE_JUMP_INDEX 5
#define JOYSTICK_STATE_SELECT_INDEX 6
#define JOYSTICK_STATE_START_INDEX 7
static bool joystick1_state[8] = { false };
static bool joystick2_state[8] = { false };

//------------------------------------------------------------------------------
// The Program
//------------------------------------------------------------------------------
class SuperMethaneBrothers
{
public:
	void on_button_press(const CL_InputEvent &key, const CL_InputState &state)
	{
		LastKey = key.id;
	}
	void on_window_close()
	{
		LastKey = CL_KEY_ESCAPE;
	}
	void on_window_resize(int proposedWidth, int proposedHeight)
	{
		if (m_WindowPtr != NULL)
		{
			CL_Rect currentRect = m_WindowPtr->get_geometry();
			float currentWidth = (float)currentRect.get_width();
			float currentHeight = (float)currentRect.get_height();
			float targetAspectRatio = ((float)SCR_WIDTH) / ((float)SCR_HEIGHT);
			int nextWidth = proposedWidth;
			int nextHeight = proposedHeight;

			float proposedAspectRatio = (float)proposedWidth / (float)proposedHeight;
			if (proposedAspectRatio > targetAspectRatio)
			{
				// Use width as reference for resize
				nextHeight = (int)(((float)proposedWidth) / targetAspectRatio);
			} 
			else
			{
				// Use height as reference for resize
				nextWidth = (int)(((float)proposedHeight) * targetAspectRatio);
			}

#ifdef WINDOW_RESIZE_DEBUG
			char buffer[160];
			sprintf(buffer, "on_window_resize w: %d h: %d targetAR: %.2f proposedAR: %.2f nextWidth: %d nextHeight: %d", proposedWidth, proposedHeight, targetAspectRatio, proposedAspectRatio, nextWidth, nextHeight);
			CL_ConsoleLogger logger = CL_ConsoleLogger();
			logger.log("window", buffer);
#endif
			m_WindowPtr->set_size(nextWidth, nextHeight, true);
		}
	}

	void on_joystick1_axis_move(const CL_InputEvent &axis, const CL_InputState &state)
	{
		#ifdef JOYSTICK_CONSOLE_DEBUG
		char buffer[80];
		sprintf(buffer, "on_axis_mov %.2f", axis.axis_pos);
		CL_ConsoleLogger logger = CL_ConsoleLogger();
		logger.log("joy1", buffer);
		#endif
		
		// Horizontal Axis
		if (axis.id == JOYSTICK_X_AXIS_ID) {
			if (axis.axis_pos < -0.5) {
				joystick1_state[JOYSTICK_STATE_LEFT_INDEX] = true;
				joystick1_state[JOYSTICK_STATE_RIGHT_INDEX] = false;
			} else if (axis.axis_pos > 0.5) {
				joystick1_state[JOYSTICK_STATE_LEFT_INDEX] = false;
				joystick1_state[JOYSTICK_STATE_RIGHT_INDEX] = true;
			} else {
				joystick1_state[JOYSTICK_STATE_LEFT_INDEX] = false;
				joystick1_state[JOYSTICK_STATE_RIGHT_INDEX] = false;
			}
		}

		// Vertical Axis
		if (axis.id == JOYSTICK_Y_AXIS_ID) {
			if (axis.axis_pos < -0.5) {
				joystick1_state[JOYSTICK_STATE_UP_INDEX] = true;
				joystick1_state[JOYSTICK_STATE_DOWN_INDEX] = false;
			} else if (axis.axis_pos > 0.5) {
				joystick1_state[JOYSTICK_STATE_UP_INDEX] = false;
				joystick1_state[JOYSTICK_STATE_DOWN_INDEX] = true;
			} else {
				joystick1_state[JOYSTICK_STATE_UP_INDEX] = false;
				joystick1_state[JOYSTICK_STATE_DOWN_INDEX] = false;
			}
		}
	}

	void on_joystick2_axis_move(const CL_InputEvent &axis, const CL_InputState &state)
	{
		#ifdef JOYSTICK_CONSOLE_DEBUG
		char buffer[80];
		sprintf(buffer, "on_axis_mov %.2f", axis.axis_pos);
		CL_ConsoleLogger logger = CL_ConsoleLogger();
		logger.log("joy2", buffer);
		#endif
		
		// Horizontal Axis
		if (axis.id == JOYSTICK_X_AXIS_ID) {
			if (axis.axis_pos < -0.5) {
				joystick2_state[JOYSTICK_STATE_LEFT_INDEX] = true;
				joystick2_state[JOYSTICK_STATE_RIGHT_INDEX] = false;
			} else if (axis.axis_pos > 0.5) {
				joystick2_state[JOYSTICK_STATE_LEFT_INDEX] = false;
				joystick2_state[JOYSTICK_STATE_RIGHT_INDEX] = true;
			} else {
				joystick2_state[JOYSTICK_STATE_LEFT_INDEX] = false;
				joystick2_state[JOYSTICK_STATE_RIGHT_INDEX] = false;
			}
		}

		// Vertical Axis
		if (axis.id == JOYSTICK_Y_AXIS_ID) {
			if (axis.axis_pos < -0.5) {
				joystick2_state[JOYSTICK_STATE_UP_INDEX] = true;
				joystick2_state[JOYSTICK_STATE_DOWN_INDEX] = false;
			} else if (axis.axis_pos > 0.5) {
				joystick2_state[JOYSTICK_STATE_UP_INDEX] = false;
				joystick2_state[JOYSTICK_STATE_DOWN_INDEX] = true;
			} else {
				joystick2_state[JOYSTICK_STATE_UP_INDEX] = false;
				joystick2_state[JOYSTICK_STATE_DOWN_INDEX] = false;
			}
		}
	}

	void on_joystick1_button_pressed(const CL_InputEvent &button, const CL_InputState &state)
	{
		#ifdef JOYSTICK_CONSOLE_DEBUG
		char buffer[80];
		if (button.type == CL_InputEvent::pressed) {
			sprintf(buffer, "button down %d", button.id);
		} else if (button.type == CL_InputEvent::released) {
			sprintf(buffer, "button up %d", button.id);
		}
		CL_ConsoleLogger logger = CL_ConsoleLogger();
		logger.log("joy1", buffer);
		#endif

		switch (button.id) {
		case JOYSTICK_FIRE_BUTTON_ID:
			if (button.type == CL_InputEvent::pressed) {
				joystick1_state[JOYSTICK_STATE_FIRE_INDEX] = true;
			} else {
				joystick1_state[JOYSTICK_STATE_FIRE_INDEX] = false;
			}
			break;
		case JOYSTICK_JUMP_BUTTON_ID:
			if (button.type == CL_InputEvent::pressed) {
				joystick1_state[JOYSTICK_STATE_JUMP_INDEX] = true;
			} else {
				joystick1_state[JOYSTICK_STATE_JUMP_INDEX] = false;
			}
			break;
		}
	}

	void on_joystick2_button_pressed(const CL_InputEvent &button, const CL_InputState &state)
	{
		#ifdef JOYSTICK_CONSOLE_DEBUG
		char buffer[80];
		if (button.type == CL_InputEvent::pressed) {
			sprintf(buffer, "button down %d", button.id);
		} else if (button.type == CL_InputEvent::released) {
			sprintf(buffer, "button up %d", button.id);
		}
		CL_ConsoleLogger logger = CL_ConsoleLogger();
		logger.log("joy2", buffer);
		#endif

		switch (button.id) {
		case JOYSTICK_FIRE_BUTTON_ID:
			if (button.type == CL_InputEvent::pressed) {
				joystick2_state[JOYSTICK_STATE_FIRE_INDEX] = true;
			} else {
				joystick2_state[JOYSTICK_STATE_FIRE_INDEX] = false;
			}
			break;
		case JOYSTICK_JUMP_BUTTON_ID:
			if (button.type == CL_InputEvent::pressed) {
				joystick2_state[JOYSTICK_STATE_JUMP_INDEX] = true;
			} else {
				joystick2_state[JOYSTICK_STATE_JUMP_INDEX] = false;
			}
			break;
		}
	}

	int main(const std::vector<CL_String> &args)
	{
		try
		{
			CL_SetupGL target_opengl2;
			CL_SetupGL1 target_opengl1;
			CL_SetupSWRender target_swrender;

			// Since SWRender is compatible and fast - Use that as the default setting options
			//target_swrender.set_current();

			target_opengl2.set_current();

			/*
			if (get_options())
			{
				switch (GLOBAL_RenderTarget)
				{
					case (opengl2):
						target_opengl2.set_current();
						break;
					case (opengl1):
						target_opengl1.set_current();
						break;
					case (swrender):
						target_swrender.set_current();
						break;
				}
			}
			else
			{
				// User quitted at the options page
				return 0;
			}
			*/

			CL_AutoPtr<CL_SetupSound> setup_sound;
			CL_AutoPtr<CL_SoundOutput> sound_output;
			CL_AutoPtr<CL_SetupMikMod> setup_mikmod;

			if (GLOBAL_SoundEnable)
			{
				setup_sound = new CL_SetupSound;
				sound_output = new CL_SoundOutput(44100);
				setup_mikmod = new CL_SetupMikMod;
			}

			const int kTargetHeight = SCR_HEIGHT * 2;//768;//1080;
			// Set the video mode
			CL_DisplayWindowDescription desc;
			desc.set_title("Super Methane Brothers");
			desc.set_size(CL_Size(SCR_WIDTH*kTargetHeight/SCR_HEIGHT,kTargetHeight), true);
			desc.set_allow_resize(true);
			/*desc.set_fullscreen(false);
			desc.show_caption(false);
			desc.set_drop_shadow(false);
			desc.show_border(false);
			*/
			//desc.set_topmost(true);
			/*
			desc.set_decorations(false);
			desc.set_tool_window(false);*/
			CL_DisplayWindow window(desc);
			m_WindowPtr = &window;

			CMethDoc Game(window);

			// Connect the Window close event
			CL_Slot slot_quit = window.sig_window_close().connect(this, &SuperMethaneBrothers::on_window_close);
			CL_Slot slot_resize = window.sig_resize().connect(this, &SuperMethaneBrothers::on_window_resize);

                        // Connect a keyboard handler to on_key_down()
			CL_Slot slot_keyboard_down = (window.get_ic().get_keyboard()).sig_key_down().connect(this, &SuperMethaneBrothers::on_button_press);

			CL_Slot slot_joystick1_axes;
			CL_Slot slot_joystick1_buttons_down;
			CL_Slot slot_joystick1_buttons_up;

			CL_Slot slot_joystick2_axes;
			CL_Slot slot_joystick2_buttons_down;
			CL_Slot slot_joystick2_buttons_up;

			int joystickCnt = window.get_ic().get_joystick_count();
			if (joystickCnt >= 1) {
				slot_joystick1_axes = (window.get_ic().get_joystick(0)).sig_axis_move().connect(this, &SuperMethaneBrothers::on_joystick1_axis_move);
				slot_joystick1_buttons_down = (window.get_ic().get_joystick(0)).sig_key_down().connect(this, &SuperMethaneBrothers::on_joystick1_button_pressed);
				slot_joystick1_buttons_up = (window.get_ic().get_joystick(0)).sig_key_up().connect(this, &SuperMethaneBrothers::on_joystick1_button_pressed);
			}

			if (joystickCnt >= 2) {
				slot_joystick2_axes = (window.get_ic().get_joystick(1)).sig_axis_move().connect(this, &SuperMethaneBrothers::on_joystick2_axis_move);
				slot_joystick2_buttons_down = (window.get_ic().get_joystick(1)).sig_key_down().connect(this, &SuperMethaneBrothers::on_joystick2_button_pressed);
				slot_joystick2_buttons_up = (window.get_ic().get_joystick(1)).sig_key_up().connect(this, &SuperMethaneBrothers::on_joystick2_button_pressed);
			}
			//

			Game.InitGame();
			Game.LoadScores();
			Game.StartGame();

			CL_GraphicContext gc = window.get_gc();

			CL_Font font(gc, "tahoma", 32);

//------------------------------------------------------------------------------
// The main game
//------------------------------------------------------------------------------

			int last_time = CL_System::get_time();

			int quit_flag = 0;
			int disable_scale_flag = 0;
			int full_screen_flag = 0;
			int on_options_screen = 0;
			int option_page = 0;
			int game_speed = 60;

			LastKey = 0;
			// Run until someone presses escape
			while (!quit_flag)
			{

//------------------------------------------------------------------------------
// Joystick Emulation and Video mode Control
//------------------------------------------------------------------------------

				JOYSTICK *jptr1;
				JOYSTICK *jptr2;
				jptr1 = &Game.m_GameTarget.m_Joy1;
				jptr2 = &Game.m_GameTarget.m_Joy2;

				if (LastKey)
				{
					if (LastKey == CL_KEY_ESCAPE) quit_flag = 1;

					if (on_options_screen)
					{
						if (LastKey == CL_KEY_SPACE)
						{
							option_page++;
							if (option_page==3)
							{
								on_options_screen = 0;	// Start the game
							}
						}
					}
				
					if (LastKey == CL_KEY_F9)
					{
						game_speed -= 20;
						if (game_speed <0) game_speed = 0;
					}
					if (LastKey == CL_KEY_F10)
					{
						game_speed += 20;
						if (game_speed >200) game_speed = 200;
					}

					jptr1->key = jptr2->key = ':';	// Fake key press (required for high score table)
					if ((LastKey >= CL_KEY_A) && (LastKey <= CL_KEY_Z)) jptr1->key = jptr2->key = LastKey - CL_KEY_A + 'A';
					if ((LastKey >= CL_KEY_0) && (LastKey <= CL_KEY_9)) jptr1->key = jptr2->key = LastKey - CL_KEY_0 + '0';
					if (LastKey == CL_KEY_SPACE) jptr1->key = jptr2->key = ' ';
					if (LastKey == CL_KEY_ENTER) jptr1->key = jptr2->key = 10;
					if (LastKey == CL_KEY_TAB)
					{
						Game.m_GameTarget.m_Game.TogglePuffBlow();
					}
					LastKey = 0;
				}

				if ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_ESCAPE)) quit_flag = 1;
				if ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_LEFT) || joystick1_state[JOYSTICK_STATE_LEFT_INDEX]) jptr1->left = 1; else jptr1->left = 0;
				if ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_RIGHT) || joystick1_state[JOYSTICK_STATE_RIGHT_INDEX]) jptr1->right = 1; else jptr1->right = 0;
				if ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_UP) || joystick1_state[JOYSTICK_STATE_UP_INDEX] || joystick1_state[JOYSTICK_STATE_JUMP_INDEX]) jptr1->up = 1; else jptr1->up = 0;
				if ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_DOWN) || joystick1_state[JOYSTICK_STATE_DOWN_INDEX]) jptr1->down = 1; else jptr1->down = 0;

				if ( ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_LCONTROL)) || ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_RCONTROL)) || joystick1_state[JOYSTICK_STATE_FIRE_INDEX])
				{
					jptr1->fire = 1;
				}else	jptr1->fire = 0;


                if ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_A) || joystick2_state[JOYSTICK_STATE_LEFT_INDEX]) jptr2->left = 1; else jptr2->left = 0;
                if ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_D) || joystick2_state[JOYSTICK_STATE_RIGHT_INDEX]) jptr2->right = 1; else jptr2->right = 0;
                if ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_W) || joystick2_state[JOYSTICK_STATE_UP_INDEX] || joystick2_state[JOYSTICK_STATE_JUMP_INDEX]) jptr2->up = 1; else jptr2->up = 0;
                if ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_S) || joystick2_state[JOYSTICK_STATE_DOWN_INDEX]) jptr2->down = 1; else jptr2->down = 0;

                if ( ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_LSHIFT)) || ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_RSHIFT)) || joystick2_state[JOYSTICK_STATE_FIRE_INDEX] )
                {
                        jptr2->fire = 1;
                }else   jptr2->fire = 0;

				// (CHEAT MODE ENABLED)
				//if ((window.get_ic().get_keyboard()).get_keycode(CL_KEY_F11)) jptr1->next_level = 1; else jptr1->next_level = 0;

								/*
				CL_InputContext &inCtx = window.get_ic();

				int joystickCnt = inCtx.get_joystick_count();
				
				if (joystickCnt >= 1) {
					CL_InputDevice &joystick1 = inCtx.get_joystick(0);
					int inputType = joystick1.get_type();
					bool didEventOccur = joystick1.poll(false);
					int axis_count = joystick1.get_axis_count();
					double xAxis = joystick1.get_axis(0);
					double yAxis = joystick1.get_axis(1);
					int buttonCnt = joystick1.get_button_count();

					//printf("x: %f.2, y: %f.2 buttonCnt: %d", xAxis, yAxis, buttonCnt);
				}
								*/

//------------------------------------------------------------------------------
// Do game main loop
//------------------------------------------------------------------------------
				gc.clear(CL_Colorf(0.0f,0.0f,0.0f));
				if (on_options_screen)
				{
					Game.DisplayOptions( window, font, option_page);
				}else
				{
					Game.MainLoop();
				}
//------------------------------------------------------------------------------
// Output the graphics
//------------------------------------------------------------------------------

				CL_KeepAlive::process();
				window.flip(1);

//------------------------------------------------------------------------------
// Control game speed
//------------------------------------------------------------------------------

				int time_diff = 0;
				do
				{
					int time_now = CL_System::get_time();
					time_diff = time_now - last_time;
					time_diff = game_speed - time_diff;
					if (time_diff > 50) CL_System::sleep(20);
				}while(time_diff > 0);
				last_time = last_time + game_speed;
				//CL_System::sleep(20);
			}
			Game.SaveScores();
		}
		catch(CL_Exception& exception)
		{
			// Create a console window for text-output if not available
			CL_ConsoleWindow console("Console");

			CL_Console::write_line("Exception caught:");
			CL_Console::write_line(exception.message);

			// Display the stack trace (if available)
			std::vector<CL_String> stacktrace = exception.get_stack_trace();
			int size = stacktrace.size();
			if (size > 0)
			{
				CL_Console::write_line("Stack Trace:");
				for (int cnt=0; cnt < size; cnt++)
				{
					CL_Console::write_line(stacktrace[cnt]);
				}
			}

			console.display_close_message();

			return -1;
		}

		return 0;
	}

	bool get_options()
	{
		//CL_DisplayWindow window("Methane Options", 640, 480);
		CL_DisplayWindow window("Methane Options", SCR_WIDTH*2, SCR_HEIGHT*2);

		// Connect the Window close event
		CL_Slot slot_quit = window.sig_window_close().connect(this, &SuperMethaneBrothers::on_window_close);

		// Connect a keyboard handler to on_key_up()
		CL_Slot slot_input_up = (window.get_ic().get_keyboard()).sig_key_up().connect(this, &SuperMethaneBrothers::on_button_press);

		// Get the graphic context
		CL_GraphicContext gc = window.get_gc();

		LastKey = 0;

		CL_Font options_font(gc, "tahoma", 20);

		while ( ! (LastKey == CL_KEY_ESCAPE || LastKey == CL_KEY_SPACE ) )
		{
			if ( (LastKey == 'a') || (LastKey == 'A') )
			{
				LastKey = 0;
				GLOBAL_SoundEnable = (GLOBAL_SoundEnable == false);
			}

			if ( (LastKey == 'w') || (LastKey == 'W') )
			{
				LastKey = 0;
				GLOBAL_RenderTarget = opengl2;
			}
			if ( (LastKey == 'e') || (LastKey == 'E') )
			{
				LastKey = 0;
				GLOBAL_RenderTarget = opengl1;
			}
			if ( (LastKey == 'r') || (LastKey == 'R') )
			{
				LastKey = 0;
				GLOBAL_RenderTarget = swrender;
			}

			gc.clear(CL_Colorf(0.0f,0.0f,0.2f));

			int ypos = 40;
			options_font.draw_text(gc, 10, ypos, "Global Options");
			ypos += 50;
			if (GLOBAL_RenderTarget == opengl2)
			{
				options_font.draw_text(gc, 10, ypos, "OpenGL V2 - Enabled.");
			}
			else
			{
				options_font.draw_text(gc, 10, ypos, "OpenGL V2 - Disabled. Press 'W' to select");
			}
			ypos += 50;
			if (GLOBAL_RenderTarget == opengl1)
			{
				options_font.draw_text(gc, 10, ypos, "OpenGL V1 - Enabled.");
			}
			else
			{
				options_font.draw_text(gc, 10, ypos, "OpenGL V1 - Disabled. Press 'E' to select");
			}
			ypos += 50;
			if (GLOBAL_RenderTarget == swrender)
			{
				options_font.draw_text(gc, 10, ypos, "Software Renderer - Enabled.");
			}
			else
			{
				options_font.draw_text(gc, 10, ypos, "Software Renderer - Disabled. Press 'R' to select");
			}
			ypos += 100;
			if (GLOBAL_SoundEnable)
			{
				options_font.draw_text(gc, 10, ypos, "Audio - Enabled. Press 'A' to toggle");
			}
			else
			{
				options_font.draw_text(gc, 10, ypos, "Audio - Disabled. Press 'A' to toggle");
			}

			ypos += 100;
			options_font.draw_text(gc, 10, ypos, "Press the spacebar to start");

			window.flip(1);
			CL_KeepAlive::process(0);
		}
		return (LastKey != CL_KEY_ESCAPE);

	}
};

// This is the Program class that is called by CL_ClanApplication
class Program
{
public:
	static int main(const std::vector<CL_String> &args)
	{
		CL_SetupCore setup_core;
		CL_SetupDisplay setup_display;

		// Start the Application
		SuperMethaneBrothers app;
		int retval = app.main(args);
		return retval;
	}
};

// Instantiate CL_ClanApplication, informing it where the Program is located
CL_ClanApplication app(&Program::main);

