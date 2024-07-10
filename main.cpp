#include "main.h"





// Main code
int main(int, char**)
{



	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// GL 4.3 + GLSL 430
	const char* glsl_version = "#version 430";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("World Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	if (window == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	if (GLEW_OK != glewInit())
	{
		cout << "GLEW initialization error" << endl;
		return false;
	}

	if (false == ortho_shader.init("ortho.vs.glsl", "ortho.fs.glsl"))
	{
		cout << "Could not load ortho shader" << endl;
		return false;
	}

	if (false == line_shader.init("lines.vs.glsl", "lines.gs.glsl", "lines.fs.glsl"))
	{
		cout << "Could not load line shader" << endl;
		return false;
	}




	uniforms.ortho_shader_uniforms.tex = glGetUniformLocation(ortho_shader.get_program(), "tex");
	uniforms.ortho_shader_uniforms.viewport_width = glGetUniformLocation(ortho_shader.get_program(), "viewport_width");
	uniforms.ortho_shader_uniforms.viewport_height = glGetUniformLocation(ortho_shader.get_program(), "viewport_height");

	uniforms.line_shader_uniforms.colour = glGetUniformLocation(line_shader.get_program(), "colour");
	uniforms.line_shader_uniforms.img_width = glGetUniformLocation(line_shader.get_program(), "img_width");
	uniforms.line_shader_uniforms.img_height = glGetUniformLocation(line_shader.get_program(), "img_height");
	uniforms.line_shader_uniforms.line_thickness = glGetUniformLocation(line_shader.get_program(), "line_thickness");



	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	// - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);

	// Our state

	ImVec4 clear_color = ImVec4(1.0, 0.5, 0.0, 1.0);

	// Main loop
	bool done = false;



	int main_tiles_width = 0;
	int main_tiles_height = 0;
	GLuint main_tiles_texture = 0;
	Mat main_tiles_img;

	int custom_brush1_width = 0;
	int custom_brush1_height = 0;
	GLuint custom_brush1_texture = 0;
	Mat custom_brush1_img;





	bool ret = LoadTextureFromFile(main_tiles_img, "goblins.png", &main_tiles_texture, &main_tiles_width, &main_tiles_height);

	if (ret == false)
	{
		cout << "Could not load goblins.png" << endl;
		return false;
	}

	ret = LoadTextureFromFile(custom_brush1_img, "custom_brush_1.png", &custom_brush1_texture, &custom_brush1_width, &custom_brush1_height);

	if (ret == false)
	{
		cout << "Could not load custom_brush_1.png" << endl;
		return false;
	}


	if (tiles_per_dimension % tiles_per_chunk_dimension != 0)
	{
		cout << "Tiles per dimension must be evenly divisible by tile chunk size" << endl;
		return false;
	}

	int num_chunks_per_map_dimension = tiles_per_dimension / tiles_per_chunk_dimension;

	background_chunks.resize(num_chunks_per_map_dimension * num_chunks_per_map_dimension);

	background_tiles.resize(tiles_per_dimension * tiles_per_dimension);

	// Initialize to use 
	for (size_t i = 0; i < tiles_per_dimension; i++)
	{
		for (size_t j = 0; j < tiles_per_dimension; j++)
		{
			const size_t background_chunk_x = i / tiles_per_chunk_dimension;
			const size_t background_chunk_y = j / tiles_per_chunk_dimension;
			const size_t background_chunk_index = background_chunk_x * num_chunks_per_map_dimension + background_chunk_y;

			background_chunks[background_chunk_index].indices.push_back(ImVec2(i, j));
	
			const size_t index = i * tiles_per_dimension + j;
			
			background_tiles[index].tile_size = 36;
			background_tiles[index].uv_min = ImVec2(0, 0);
			background_tiles[index].uv_max = ImVec2(float(background_tiles[index].tile_size) / main_tiles_width, float(background_tiles[index].tile_size) / main_tiles_height);
		}
	}



	for (size_t k = 0; k < num_chunks_per_map_dimension; k++)
	{
		for (size_t l = 0; l < num_chunks_per_map_dimension; l++)
		{
			size_t index = k * num_chunks_per_map_dimension + l;

			float min_x = FLT_MAX;
			float max_x = -FLT_MAX;
			float min_y = FLT_MAX;
			float max_y = -FLT_MAX;

			for (size_t m = 0; m < background_chunks[index].indices.size(); m++)
			{
				ImVec2 indices = background_chunks[index].indices[m];
				size_t tile_index = indices.x * tiles_per_dimension + indices.y;

				float pixel_x = int(image_anchor.x) + int(indices.x) * background_tiles[tile_index].tile_size;
				float pixel_y = int(image_anchor.y) + int(indices.y) * background_tiles[tile_index].tile_size;

				if (pixel_x < min_x)
					min_x = pixel_x;

				if (pixel_x > max_x)
					max_x = pixel_x;

				if (pixel_y < min_y)
					min_y = pixel_y;

				if (pixel_y > max_y)
					max_y = pixel_y;
			}

			background_chunks[index].region_min = ImVec2(min_x, min_y);
			background_chunks[index].region_max = ImVec2(max_x, max_y);
		}
	}






	int window_w = 0, window_h = 0;
	SDL_GetWindowSize(window, &window_w, &window_h);

	image_anchor.x = 0;// float(window_w) / 2.0 - 36.0f * float(tiles_per_dimension) / 2.0f;
	image_anchor.y = 0;// float(window_h) / 2.0 - 36.0f * float(tiles_per_dimension) / 2.0f;

	mt19937 generator((unsigned int)time(0));
	uniform_real_distribution<float> distribution(0.0, 1.0);

	vector<size_t> prev_painted_indices;

	set<size_t> selected_indices; // use a set instead, for faster lookup
	ImVec2 selected_start;
	ImVec2 selected_end;



#define TOOL_PAINT 0
#define TOOL_PAINT_SQUARE 1
#define TOOL_PAINT_CUSTOM 2
#define TOOL_SELECT 3
#define TOOL_SELECT_ADD 4
#define TOOL_SELECT_SUBTRACT 5
#define TOOL_PAN 6

	int tool = 0;

	int brush_size = 20;


	vector<int> prev_tools;

	while (!done)
	{
		bool make_selection = false;

		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		last_mousewheel = 0;



		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;

			if (event.type == SDL_MOUSEWHEEL)
				last_mousewheel = (float)event.wheel.y;

			prev_tools.push_back(tool);

			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
			{
				tool = TOOL_PAN;
			}
			else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE)
			{
				for (size_t i = 0; i < prev_tools.size(); i++)
				{
					if (prev_tools[i] != TOOL_PAN)
						tool = prev_tools[i];
				}

				prev_tools.clear();
			}





			if ((tool == TOOL_SELECT || tool == TOOL_SELECT_ADD || tool == TOOL_SELECT_SUBTRACT) && event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					int x, y;
					SDL_GetMouseState(&x, &y);

					selected_start = ImVec2((float)x, (float)y);
					selected_end = ImVec2((float)x, (float)y);
				}
			}

			if ((tool == TOOL_SELECT || tool == TOOL_SELECT_ADD || tool == TOOL_SELECT_SUBTRACT) && event.type == SDL_MOUSEBUTTONUP)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					int x, y;
					SDL_GetMouseState(&x, &y);

					selected_end = ImVec2((float)x, (float)y);

					make_selection = true;
				}
			}
		}



		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// Only pan image if not hovering over an ImGui window
		bool hovered = false;

		ImGui::Begin("image", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

		if (ImGui::IsWindowHovered())
			hovered = true;

		const ImVec2 img_size = { float(main_tiles_width), float(main_tiles_height) };

		static char str0[128] = "36";
		ImGui::InputText("Tile size", str0, IM_ARRAYSIZE(str0));

		istringstream iss(str0);
		int block_size = 0;
		iss >> block_size;

		{
			ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
			ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

			ImGui::Image((void*)(intptr_t)main_tiles_texture, img_size, uv_min, uv_max, tint_col, border_col);
		}

		const ImVec2 mousePositionAbsolute = ImGui::GetMousePos();
		const ImVec2 screenPositionAbsolute = ImGui::GetItemRectMin();
		const ImVec2 mousePositionRelative = ImVec2(mousePositionAbsolute.x - screenPositionAbsolute.x, mousePositionAbsolute.y - screenPositionAbsolute.y);

		//ImVec2 img_block = ImVec2(floor(mousePositionRelative.x / block_size), floor(mousePositionRelative.y / block_size));
		//cout << img_block.x << " " << img_block.y << endl;


		bool left_clicked = false;
		bool right_clicked = false;

		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			left_clicked = true;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			right_clicked = true;
		}

		ImGui::End();





		ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

		if (ImGui::IsWindowHovered())
			hovered = true;


		ImGui::RadioButton("Circle Paint", &tool, TOOL_PAINT);
		ImGui::RadioButton("Square Paint", &tool, TOOL_PAINT_SQUARE);
		ImGui::RadioButton("Custom Paint", &tool, TOOL_PAINT_CUSTOM);

		ImGui::RadioButton("Select", &tool, TOOL_SELECT);
		ImGui::RadioButton("Select Add", &tool, TOOL_SELECT_ADD);
		ImGui::RadioButton("Select Subtract", &tool, TOOL_SELECT_SUBTRACT);

		static char str1[128] = "5";
		ImGui::InputText("Brush size", str1, IM_ARRAYSIZE(str1));

		istringstream iss1(str1);
		iss1 >> brush_size;


		ImGui::End();











		ImGui::Begin("Left Brush", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

		if (ImGui::IsWindowHovered())
			hovered = true;

		if (ImGui::Button("Add"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			left_add_button_func();

		for (int i = 0; i < left_strings.size(); i++)
		{
			const ImVec2 thumbnail_img_size = { float(block_size), float(block_size) };

			if (left_clicked && i == left_selected)
			{
				size_t x = size_t(mousePositionRelative.x) % block_size;
				size_t y = size_t(mousePositionRelative.y) % block_size;

				float u_start = (mousePositionRelative.x - x) / img_size.x;
				float v_start = (mousePositionRelative.y - y) / img_size.y;

				float u_end = block_size / img_size.x + u_start;
				float v_end = block_size / img_size.y + v_start;

				left_uv_mins[i] = ImVec2(u_start, v_start);
				left_uv_maxs[i] = ImVec2(u_end, v_end);

				//ImVec2 img_block = ImVec2(floor(mousePositionRelative.x / block_size), floor(mousePositionRelative.y / block_size));
				//cout << img_block.x << " " << img_block.y << endl;
				//left_indices[i] = img_block;
			}

			const ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			const ImVec4 selected_border_col = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
			const ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			if (i == left_selected)
				ImGui::Image((void*)(intptr_t)main_tiles_texture, thumbnail_img_size, left_uv_mins[i], left_uv_maxs[i], tint_col, selected_border_col);
			else
				ImGui::Image((void*)(intptr_t)main_tiles_texture, thumbnail_img_size, left_uv_mins[i], left_uv_maxs[i], tint_col, border_col);

			if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
				left_selected = i;



			ImGui::SameLine();

			if (ImGui::Button((string("Remove ") + to_string(i)).c_str()))                         // Buttons return true when clicked (most widgets return true when edited/activated)
				left_remove_button_func(i);

			ImGui::SameLine();

			string x = "Weight " + to_string(i);

			ImGui::PushItemWidth(80);
			ImGui::InputText(x.c_str(), &left_strings[i]);
			ImGui::PopItemWidth();
		}



		ImGui::End();






		ImGui::Begin("Right Brush", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

		if (ImGui::IsWindowHovered())
			hovered = true;

		if (ImGui::Button("Add"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			right_add_button_func();

		for (int i = 0; i < right_strings.size(); i++)
		{
			const ImVec2 thumbnail_img_size = { float(block_size), float(block_size) };

			if (right_clicked && i == right_selected)
			{
				size_t x = size_t(mousePositionRelative.x) % block_size;
				size_t y = size_t(mousePositionRelative.y) % block_size;

				float u_start = (mousePositionRelative.x - x) / img_size.x;
				float v_start = (mousePositionRelative.y - y) / img_size.y;

				float u_end = block_size / img_size.x + u_start;
				float v_end = block_size / img_size.y + v_start;

				right_uv_mins[i] = ImVec2(u_start, v_start);
				right_uv_maxs[i] = ImVec2(u_end, v_end);
			}

			const ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			const ImVec4 selected_border_col = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
			const ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			if (i == right_selected)
				ImGui::Image((void*)(intptr_t)main_tiles_texture, thumbnail_img_size, right_uv_mins[i], right_uv_maxs[i], tint_col, selected_border_col);
			else
				ImGui::Image((void*)(intptr_t)main_tiles_texture, thumbnail_img_size, right_uv_mins[i], right_uv_maxs[i], tint_col, border_col);

			if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
				right_selected = i;

			ImGui::SameLine();

			if (ImGui::Button((string("Remove ") + to_string(i)).c_str()))
				right_remove_button_func(i);

			ImGui::SameLine();

			string x = "Weight " + to_string(i);

			ImGui::PushItemWidth(80);
			ImGui::InputText(x.c_str(), &right_strings[i]);
			ImGui::PopItemWidth();


		}


		ImGui::End();






		// Rendering
		ImGui::Render();

		if (!hovered)
		{
			if (last_mousewheel < 0)
				zoom_factor *= 0.5;// last_mousewheel * 0.1f;
			else if (last_mousewheel > 0)
				zoom_factor *= 2.0;

			//if (last_mousewheel != 0)
			//{
			//	//image_anchor.x =  36.0f * float(tiles_per_dimension) / 2.0f  - float(window_w) / 2.0;
			//	//image_anchor.y =  36.0f * float(tiles_per_dimension) / 2.0f - float(window_h) / 2.0;
			//}
		}

		if (!hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0) && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
		{
			ImVec2 motion = ImGui::GetMouseDragDelta();
			image_anchor.x += motion.x / zoom_factor;
			image_anchor.y += -motion.y / zoom_factor;

			ImGui::ResetMouseDragDelta();
		}


		if (!hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0))
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			selected_end = ImVec2((float)x, (float)y);
		}


		// Paint using left mouse button
		if ((tool == TOOL_PAINT || tool == TOOL_PAINT_SQUARE) && !hovered && (ImGui::IsMouseDown(ImGuiMouseButton_Left)) && left_strings.size() > 0)
		{
			vector<float> weights;
			float total = 0;

			for (int i = 0; i < left_strings.size(); i++)
			{
				const float weight = stof(left_strings[i]);
				weights.push_back(weight);
				total += weight;
			}

			if (total != 0.0f && total != -0.0f)
			{
				for (int i = 0; i < left_strings.size(); i++)
					weights[i] /= total;
			}

			ImVec2 centre_index;
			glm::vec3 centre_location;
			bool inside = false;

			size_t brush_in_use = 0;

			vector<size_t> curr_painted_indices;

			set<size_t> to_draw;

			float width_factor = 1;// zoom_factor* tiles_per_dimension* block_size * 0.5f;// (io.DisplaySize.x* brush_size)* zoom_factor;// (float)brush_size / io.DisplaySize.x;
			float height_factor = 1;// zoom_factor* tiles_per_dimension* block_size * 0.5f;// (float)brush_size / io.DisplaySize.y;

			float max_brush_size = brush_size;

			if (custom_brush1_width > max_brush_size)
				max_brush_size = custom_brush1_width;

			if (custom_brush1_height > max_brush_size)
				max_brush_size = custom_brush1_height;

			if (max_brush_size >= io.DisplaySize.x)
				width_factor = (float)max_brush_size / io.DisplaySize.x;

			if (max_brush_size >= io.DisplaySize.y)
				height_factor = (float)max_brush_size / io.DisplaySize.y;

			width_factor *= 2.0f / zoom_factor;
			height_factor *= 2.0f / zoom_factor;


			// Find brush centre
			for (size_t i = 0; i < tiles_per_dimension; i++)
			{
				for (size_t j = 0; j < tiles_per_dimension; j++)
				{
					size_t index = i * tiles_per_dimension + j;

					int pixel_x = int(image_anchor.x) + int(i) * background_tiles[index].tile_size;
					int pixel_y = int(image_anchor.y) + int(j) * background_tiles[index].tile_size;

					quad q;
					q.vertices[0].x = pixel_x;
					q.vertices[0].y = pixel_y;
					q.vertices[1].x = pixel_x;
					q.vertices[1].y = pixel_y + background_tiles[index].tile_size;
					q.vertices[2].x = pixel_x + background_tiles[index].tile_size;
					q.vertices[2].y = pixel_y + background_tiles[index].tile_size;
					q.vertices[3].x = pixel_x + background_tiles[index].tile_size;
					q.vertices[3].y = pixel_y;

					glm::vec3 quad_centre = (q.vertices[0] + q.vertices[1] + q.vertices[2] + q.vertices[3]) * 0.25f;

					complex<float> v0w(static_cast<float>(quad_centre.x), static_cast<float>(quad_centre.y));
					complex<float> v0ndc = get_ndc_coords_from_window_coords((int)io.DisplaySize.x, (int)io.DisplaySize.y, v0w);

					//cout << width_factor << " " << height_factor << endl;

					if (v0ndc.real() < -width_factor || v0ndc.real() > width_factor || v0ndc.imag() < -height_factor || v0ndc.imag() > height_factor)
					{
						continue;
					}

					bool found_prev_index = false;

					for (size_t k = 0; k < prev_painted_indices.size(); k++)
					{
						if (prev_painted_indices[k] == index)
						{
							found_prev_index = true;
							break;
						}
					}

					if (found_prev_index)
						continue;



					vector<quad> quads;

					int x, y;
					SDL_GetMouseState(&x, &y);

					inside = draw_textured_quad(true, x, y, quads, ortho_shader.get_program(), int(image_anchor.x) + int(i) * background_tiles[index].tile_size, int(image_anchor.y) + int(j) * background_tiles[index].tile_size, background_tiles[index].tile_size, (int)io.DisplaySize.x, (int)io.DisplaySize.y, main_tiles_texture, background_tiles[index].uv_min, background_tiles[index].uv_max);

					if (quads.size() > 0)
					{
						glm::vec3 quad_centre = (quads[0].vertices[0] + quads[0].vertices[1] + quads[0].vertices[2] + quads[0].vertices[3]) * 0.25f;

						complex<float> v0w(static_cast<float>(quad_centre.x), static_cast<float>(quad_centre.y));
						complex<float> v0ndc = get_ndc_coords_from_window_coords((int)io.DisplaySize.x, (int)io.DisplaySize.y, v0w);

						size_t count = 0;

						float width_factor = 1.0f;// (float)brush_size / io.DisplaySize.x;
						float height_factor = 1.0f;// (float)brush_size / io.DisplaySize.y;

						//cout << width_factor << " " << height_factor << endl;

						if (v0ndc.real() < -width_factor || v0ndc.real() > width_factor || v0ndc.imag() < -height_factor || v0ndc.imag() > height_factor)
						{
							continue;
						}
					}

					if (inside)
					{
						const float r = distribution(generator);

						float sub_total = 0;

						for (int k = 0; k < left_strings.size(); k++)
						{
							sub_total += weights[k];

							if (r <= sub_total)
							{
								brush_in_use = k;
								break;
							}
						}

						centre_index = ImVec2((float)i, (float)j);
						centre_location = glm::vec3((quads[0].vertices[0] + quads[0].vertices[1] + quads[0].vertices[2] + quads[0].vertices[3]) * 0.25f);

						if (selected_indices.size() == 0 || selected_indices.end() != std::find(selected_indices.begin(), selected_indices.end(), index))
						{
							background_tiles[index].uv_min = left_uv_mins[brush_in_use];
							background_tiles[index].uv_max = left_uv_maxs[brush_in_use];
						}

						to_draw.insert(index);

						curr_painted_indices.push_back(index);

						// Abort early
						i = j = tiles_per_dimension;
						break;


					}
				}
			}

			// if found a brush centre
			// then go through the other tiles to see if it's
			// within reach of the brush size
			if (inside == true)
			{
				for (size_t i = 0; i < tiles_per_dimension; i++)
				{
					for (size_t j = 0; j < tiles_per_dimension; j++)
					{
						size_t index = i * tiles_per_dimension + j;

						int x = int(image_anchor.x) + int(i) * background_tiles[index].tile_size;
						int y = int(image_anchor.y) + int(j) * background_tiles[index].tile_size;

						quad q;
						q.vertices[0].x = x;
						q.vertices[0].y = y;
						q.vertices[1].x = x;
						q.vertices[1].y = y + background_tiles[index].tile_size;
						q.vertices[2].x = x + background_tiles[index].tile_size;
						q.vertices[2].y = y + background_tiles[index].tile_size;
						q.vertices[3].x = x + background_tiles[index].tile_size;
						q.vertices[3].y = y;

						glm::vec3 quad_centre = (q.vertices[0] + q.vertices[1] + q.vertices[2] + q.vertices[3]) * 0.25f;

						complex<float> v0w(static_cast<float>(quad_centre.x), static_cast<float>(quad_centre.y));
						complex<float> v0ndc = get_ndc_coords_from_window_coords((int)io.DisplaySize.x, (int)io.DisplaySize.y, v0w);

						//cout << width_factor << " " << height_factor << endl;

						if (v0ndc.real() < -width_factor || v0ndc.real() > width_factor || v0ndc.imag() < -height_factor || v0ndc.imag() > height_factor)
						{
							continue;
						}



						bool found_prev_index = false;

						for (size_t k = 0; k < prev_painted_indices.size(); k++)
						{
							if (prev_painted_indices[k] == index)
							{
								found_prev_index = true;
								break;
							}
						}

						if (found_prev_index)
							continue;







						if (tool == TOOL_PAINT)
						{
							glm::vec3 a((float)i, (float)j, 0);
							glm::vec3 b((float)centre_index.x, (float)centre_index.y, 0);

							if (distance(a, b) <= (brush_size * 0.5))
								to_draw.insert(index);
						}
						else if (tool == TOOL_PAINT_SQUARE)
						{
							if (abs(i - centre_index.x) <= (brush_size * 0.5) && abs(j - centre_index.y) <= (brush_size) * 0.5)// && !found_prev_index)
								to_draw.insert(index);
						}
					}
				}

				prev_painted_indices = curr_painted_indices;
			}

			for (size_t i = 0; i < tiles_per_dimension; i++)
			{
				for (size_t j = 0; j < tiles_per_dimension; j++)
				{
					size_t index = i * tiles_per_dimension + j;

					if (to_draw.end() == find(to_draw.begin(), to_draw.end(), index))
						continue;

					size_t brush_in_use = 0;

					const float r = distribution(generator);

					float sub_total = 0;

					for (int k = 0; k < left_strings.size(); k++)
					{
						sub_total += weights[k];

						if (r <= sub_total)
						{
							brush_in_use = k;
							break;
						}
					}

					curr_painted_indices.push_back(index);

					if (selected_indices.size() == 0 || selected_indices.end() != std::find(selected_indices.begin(), selected_indices.end(), index))
					{
						background_tiles[index].uv_min = left_uv_mins[brush_in_use];
						background_tiles[index].uv_max = left_uv_maxs[brush_in_use];
					}
				}
			}

			to_draw.clear();
		}





		if ((tool == TOOL_SELECT || tool == TOOL_SELECT_ADD || tool == TOOL_SELECT_SUBTRACT) && make_selection && !hovered)// && !ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
		{
			make_selection = false;

			if (tool == TOOL_SELECT && prev_tools.size() > 0 && prev_tools[prev_tools.size() - 1] == TOOL_SELECT)
				selected_indices.clear();

			glm::vec3 start_chunk;// = glm::vec3(num_chunks_per_map_dimension - 1, num_chunks_per_map_dimension - 1, 0);
			glm::vec3 end_chunk;// = glm::vec3(0.0f, 0.0f, 0.0f);

			for (size_t k = 0; k < num_chunks_per_map_dimension; k++)
			{
				for (size_t l = 0; l < num_chunks_per_map_dimension; l++)
				{
					size_t index = k * num_chunks_per_map_dimension + l;

					vector<glm::vec3> points;
					points.push_back(glm::vec3(background_chunks[index].region_min.x, background_chunks[index].region_min.y, 0));
					points.push_back(glm::vec3(background_chunks[index].region_min.x, background_chunks[index].region_max.y, 0));
					points.push_back(glm::vec3(background_chunks[index].region_max.x, background_chunks[index].region_max.y, 0));
					points.push_back(glm::vec3(background_chunks[index].region_max.x, background_chunks[index].region_min.y, 0));

					quad q;
					q.vertices[0] = points[0];
					q.vertices[1] = points[1];
					q.vertices[2] = points[2];
					q.vertices[3] = points[3];

					glm::vec3 ss;
					ss.x = selected_start.x;
					ss.y = selected_start.y;

					if (point_in_quad(ss, q))
					//if (point_in_polygon(ss, points))
					{
						cout << "found start_chunk" << endl;

						start_chunk = glm::vec3(k, l, 0);
						k = l = num_chunks_per_map_dimension;
						break;
					}
				}
			}

			for (size_t k = 0; k < num_chunks_per_map_dimension; k++)
			{
				for (size_t l = 0; l < num_chunks_per_map_dimension; l++)
				{
					size_t index = k * num_chunks_per_map_dimension + l;

					vector<glm::vec3> points;
					points.push_back(glm::vec3(background_chunks[index].region_min.x, background_chunks[index].region_min.y, 0));
					points.push_back(glm::vec3(background_chunks[index].region_min.x,  background_chunks[index].region_max.y, 0));
					points.push_back(glm::vec3(background_chunks[index].region_max.x,  background_chunks[index].region_max.y, 0));
					points.push_back(glm::vec3(background_chunks[index].region_max.x,  background_chunks[index].region_min.y, 0));

					glm::vec3 se;
					se.x = selected_end.x;
					se.y = selected_end.y;

					quad q;
					q.vertices[0] = points[0];
					q.vertices[1] = points[1];
					q.vertices[2] = points[2];
					q.vertices[3] = points[3];

					if (point_in_quad(se, q))
					//if (point_in_polygon(se, points))
					{
						cout << "found end_chunk" << endl;
						end_chunk = glm::vec3(k, l, 0);
						k = l = num_chunks_per_map_dimension;
						break;
					}
				}
			}

			if (end_chunk.x < start_chunk.x)
			{
				float temp = end_chunk.x;
				end_chunk.x = start_chunk.x;
				start_chunk.x = temp;
			}

			if (end_chunk.y < start_chunk.y)
			{
				float temp = end_chunk.y;
				end_chunk.y = start_chunk.y;
				start_chunk.y = temp;
			}

			cout << start_chunk.x << " " << end_chunk.x << endl;
			cout << start_chunk.y << " " << end_chunk.y << endl;

			for (size_t k = start_chunk.x; k <= end_chunk.x; k++)
			{
				for (size_t l = start_chunk.y; l <= end_chunk.y; l++)
				{
					size_t chunk_index = k * num_chunks_per_map_dimension + l;

					for (size_t m = 0; m < background_chunks[chunk_index].indices.size(); m++)
					{
						size_t i = background_chunks[chunk_index].indices[m].x;
						size_t j = background_chunks[chunk_index].indices[m].y;

						size_t index = i * tiles_per_dimension + j;

						const float x = ((image_anchor.x) + int(i) * background_tiles[index].tile_size);
						const float y = ((image_anchor.y) + int(j) * background_tiles[index].tile_size);

						complex<float> v0w(static_cast<float>(x), static_cast<float>(y));
						complex<float> v1w(static_cast<float>(x), static_cast<float>(y + background_tiles[index].tile_size));
						complex<float> v2w(static_cast<float>(x + background_tiles[index].tile_size), static_cast<float>(y + background_tiles[index].tile_size));
						complex<float> v3w(static_cast<float>(x + background_tiles[index].tile_size), static_cast<float>(y));

						v0w.real(v0w.real() * zoom_factor);
						v0w.imag(v0w.imag() * zoom_factor);
						v1w.real(v1w.real() * zoom_factor);
						v1w.imag(v1w.imag() * zoom_factor);
						v2w.real(v2w.real() * zoom_factor);
						v2w.imag(v2w.imag() * zoom_factor);
						v3w.real(v3w.real() * zoom_factor);
						v3w.imag(v3w.imag() * zoom_factor);

						quad q;
						q.vertices[0].x = v0w.real();
						q.vertices[0].y = v0w.imag();
						q.vertices[1].x = v1w.real();
						q.vertices[1].y = v1w.imag();
						q.vertices[2].x = v2w.real();
						q.vertices[2].y = v2w.imag();
						q.vertices[3].x = v3w.real();
						q.vertices[3].y = v3w.imag();

						glm::vec3 quad_centre = (q.vertices[0] + q.vertices[1] + q.vertices[2] + q.vertices[3]) * 0.25f;

						vector<glm::vec3> points;
						points.push_back(glm::vec3(selected_start.x, (int)io.DisplaySize.y - selected_start.y, 0));
						points.push_back(glm::vec3(selected_start.x, (int)io.DisplaySize.y - selected_end.y, 0));
						points.push_back(glm::vec3(selected_end.x, (int)io.DisplaySize.y - selected_end.y, 0));
						points.push_back(glm::vec3(selected_end.x, (int)io.DisplaySize.y - selected_start.y, 0));

						if (point_in_polygon(quad_centre, points) ||
							point_in_polygon(q.vertices[0], points) ||
							point_in_polygon(q.vertices[1], points) ||
							point_in_polygon(q.vertices[2], points) ||
							point_in_polygon(q.vertices[3], points))
						{
							if (tool == TOOL_SELECT_SUBTRACT)
								selected_indices.erase(index);
							else
								selected_indices.insert(index);
						}
					}
				}
			}

	//		for (size_t k = 0; k < background_chunks.size(); k++)
	//		{
	//			for (size_t l = 0; l < background_chunks[k].indices.size(); l++)
	//			{
	//				size_t i = background_chunks[k].indices[l].x;
	//				size_t j = background_chunks[k].indices[l].y;

	//				size_t index = i * tiles_per_dimension + j;

	//				const float x = ((image_anchor.x) + int(i) * background_tiles[index].tile_size);
	//				const float y = ((image_anchor.y) + int(j) * background_tiles[index].tile_size);

	///*				if (x < 0 || x >(int)io.DisplaySize.x / zoom_factor)
	//					break;

	//				if (y < 0 || y >(int)io.DisplaySize.y / zoom_factor)
	//					break;*/

	//				complex<float> v0w(static_cast<float>(x), static_cast<float>(y));
	//				complex<float> v1w(static_cast<float>(x), static_cast<float>(y + background_tiles[index].tile_size));
	//				complex<float> v2w(static_cast<float>(x + background_tiles[index].tile_size), static_cast<float>(y + background_tiles[index].tile_size));
	//				complex<float> v3w(static_cast<float>(x + background_tiles[index].tile_size), static_cast<float>(y));

	//				v0w.real(v0w.real() * zoom_factor);
	//				v0w.imag(v0w.imag() * zoom_factor);
	//				v1w.real(v1w.real() * zoom_factor);
	//				v1w.imag(v1w.imag() * zoom_factor);
	//				v2w.real(v2w.real() * zoom_factor);
	//				v2w.imag(v2w.imag() * zoom_factor);
	//				v3w.real(v3w.real() * zoom_factor);
	//				v3w.imag(v3w.imag() * zoom_factor);

	//				quad q;
	//				q.vertices[0].x = v0w.real();
	//				q.vertices[0].y = v0w.imag();
	//				q.vertices[1].x = v1w.real();
	//				q.vertices[1].y = v1w.imag();
	//				q.vertices[2].x = v2w.real();
	//				q.vertices[2].y = v2w.imag();
	//				q.vertices[3].x = v3w.real();
	//				q.vertices[3].y = v3w.imag();

	//				glm::vec3 quad_centre = (q.vertices[0] + q.vertices[1] + q.vertices[2] + q.vertices[3]) * 0.25f;

	//				vector<glm::vec3> points;
	//				points.push_back(glm::vec3(selected_start.x, (int)io.DisplaySize.y - selected_start.y, 0));
	//				points.push_back(glm::vec3(selected_start.x, (int)io.DisplaySize.y - selected_end.y, 0));
	//				points.push_back(glm::vec3(selected_end.x, (int)io.DisplaySize.y - selected_end.y, 0));
	//				points.push_back(glm::vec3(selected_end.x, (int)io.DisplaySize.y - selected_start.y, 0));

	//				if (point_in_polygon(quad_centre, points) ||
	//					point_in_polygon(q.vertices[0], points) ||
	//					point_in_polygon(q.vertices[1], points) ||
	//					point_in_polygon(q.vertices[2], points) ||
	//					point_in_polygon(q.vertices[3], points))
	//				{
	//					if (tool == TOOL_SELECT_SUBTRACT)
	//						selected_indices.erase(index);
	//					else
	//						selected_indices.insert(index);
	//				}
	//			}
	//		}

			//for (size_t i = 0; i < tiles_per_dimension; i++)
			//{
			//	for (size_t j = 0; j < tiles_per_dimension; j++)
			//	{

			//	}
			//}
		}


		//if (!hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0))
		//{
		//	// paint using right mouse button
		//}

		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		vector<float> vertex_data;
		vector<GLuint> index_data;

		for (size_t i = 0; i < tiles_per_dimension; i++)
		{
			for (size_t j = 0; j < tiles_per_dimension; j++)
			{
				size_t index = i * tiles_per_dimension + j;

				int x = int(image_anchor.x) + int(i) * background_tiles[index].tile_size;
				int y = int(image_anchor.y) + int(j) * background_tiles[index].tile_size;

				bool saved_data = get_quad_ndc_data(vertex_data, index_data, x, y, background_tiles[index].tile_size, (int)io.DisplaySize.x, (int)io.DisplaySize.y, background_tiles[index].uv_min, background_tiles[index].uv_max);
			}
		}

		draw_quad_ndc_data(vertex_data, index_data, main_tiles_texture, (int)io.DisplaySize.x, (int)io.DisplaySize.y);



		for (size_t i = 0; i < tiles_per_dimension; i++)
		{
			for (size_t j = 0; j < tiles_per_dimension; j++)
			{
				size_t index = i * tiles_per_dimension + j;


				const float x = int(image_anchor.x) + int(i) * background_tiles[index].tile_size;
				const float y = int(image_anchor.y) + int(j) * background_tiles[index].tile_size;

				complex<float> v0w(static_cast<float>(x), static_cast<float>(y));
				complex<float> v1w(static_cast<float>(x), static_cast<float>(y + background_tiles[index].tile_size));
				complex<float> v2w(static_cast<float>(x + background_tiles[index].tile_size), static_cast<float>(y + background_tiles[index].tile_size));
				complex<float> v3w(static_cast<float>(x + background_tiles[index].tile_size), static_cast<float>(y));

				v0w.real(v0w.real() * zoom_factor);
				v0w.imag(v0w.imag() * zoom_factor);
				v1w.real(v1w.real() * zoom_factor);
				v1w.imag(v1w.imag() * zoom_factor);
				v2w.real(v2w.real() * zoom_factor);
				v2w.imag(v2w.imag() * zoom_factor);
				v3w.real(v3w.real() * zoom_factor);
				v3w.imag(v3w.imag() * zoom_factor);

				quad q;
				q.vertices[0].x = v0w.real();
				q.vertices[0].y = v0w.imag();
				q.vertices[1].x = v1w.real();
				q.vertices[1].y = v1w.imag();
				q.vertices[2].x = v2w.real();
				q.vertices[2].y = v2w.imag();
				q.vertices[3].x = v3w.real();
				q.vertices[3].y = v3w.imag();

				//if (x >= 0 && x <= (int)io.DisplaySize.x && y >= 0 && y <= (int)io.DisplaySize.y)
				if (zoom_factor > 0.5)
					draw_quad_line_loop(glm::vec3(0.1, 0.1, 0.1), (int)io.DisplaySize.x, (int)io.DisplaySize.y, 1.0, q);
			}
		}













		for (size_t i = 0; i < tiles_per_dimension; i++)
		{
			for (size_t j = 0; j < tiles_per_dimension; j++)
			{
				size_t index = i * tiles_per_dimension + j;

				if (selected_indices.end() != std::find(selected_indices.begin(), selected_indices.end(), index))
				{
					const float x = ((image_anchor.x) + int(i) * background_tiles[index].tile_size);
					const float y = ((image_anchor.y) + int(j) * background_tiles[index].tile_size);

					complex<float> v0w(static_cast<float>(x), static_cast<float>(y));
					complex<float> v1w(static_cast<float>(x), static_cast<float>(y + background_tiles[index].tile_size));
					complex<float> v2w(static_cast<float>(x + background_tiles[index].tile_size), static_cast<float>(y + background_tiles[index].tile_size));
					complex<float> v3w(static_cast<float>(x + background_tiles[index].tile_size), static_cast<float>(y));

					v0w.real(v0w.real() * zoom_factor);
					v0w.imag(v0w.imag() * zoom_factor);
					v1w.real(v1w.real() * zoom_factor);
					v1w.imag(v1w.imag() * zoom_factor);
					v2w.real(v2w.real() * zoom_factor);
					v2w.imag(v2w.imag() * zoom_factor);
					v3w.real(v3w.real() * zoom_factor);
					v3w.imag(v3w.imag() * zoom_factor);

					quad q;
					q.vertices[0].x = v0w.real();
					q.vertices[0].y = v0w.imag();
					q.vertices[1].x = v1w.real();
					q.vertices[1].y = v1w.imag();
					q.vertices[2].x = v2w.real();
					q.vertices[2].y = v2w.imag();
					q.vertices[3].x = v3w.real();
					q.vertices[3].y = v3w.imag();

					draw_quad_line_loop(glm::vec3(0, 0, 1), (int)io.DisplaySize.x, (int)io.DisplaySize.y, 2.0, q);
				}
			}
		}


		// Draw selected outline 
		if ((tool == TOOL_SELECT || tool == TOOL_SELECT_ADD || tool == TOOL_SELECT_SUBTRACT) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			quad q;

			complex<float> v0w(static_cast<float>(selected_start.x), static_cast<float>((int)io.DisplaySize.y - selected_start.y));
			complex<float> v1w(static_cast<float>(selected_start.x), static_cast<float>((int)io.DisplaySize.y - selected_end.y));
			complex<float> v2w(static_cast<float>(selected_end.x), static_cast<float>((int)io.DisplaySize.y - selected_end.y));
			complex<float> v3w(static_cast<float>(selected_end.x), static_cast<float>((int)io.DisplaySize.y - selected_start.y));

			q.vertices[0].x = v0w.real();
			q.vertices[0].y = v0w.imag();
			q.vertices[1].x = v1w.real();
			q.vertices[1].y = v1w.imag();
			q.vertices[2].x = v2w.real();
			q.vertices[2].y = v2w.imag();
			q.vertices[3].x = v3w.real();
			q.vertices[3].y = v3w.imag();

			draw_quad_line_loop(glm::vec3(0, 0, 1), (int)io.DisplaySize.x, (int)io.DisplaySize.y, 4.0, q);
		}

		// Draw brush outline
		if (tool == TOOL_PAINT)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			glm::vec3 pos(x, (int)io.DisplaySize.y - y, 0);

			draw_circle_line_loop(glm::vec3(1, 1, 1), (int)io.DisplaySize.x, (int)io.DisplaySize.y, 4.0, pos, zoom_factor * (float)brush_size * block_size * 0.5f, 20);
		}
		else if (tool == TOOL_PAINT_SQUARE)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			quad q;

			q.vertices[0].x = x - zoom_factor * (float)brush_size * block_size * 0.5f;
			q.vertices[0].y = (int)io.DisplaySize.y - y - zoom_factor * (float)brush_size * block_size * 0.5f;
			q.vertices[1].x = x - zoom_factor * (float)brush_size * block_size * 0.5f;
			q.vertices[1].y = (int)io.DisplaySize.y - y + zoom_factor * (float)brush_size * block_size * 0.5f;
			q.vertices[2].x = x + zoom_factor * (float)brush_size * block_size * 0.5f;
			q.vertices[2].y = (int)io.DisplaySize.y - y + zoom_factor * (float)brush_size * block_size * 0.5f;
			q.vertices[3].x = x + zoom_factor * (float)brush_size * block_size * 0.5f;
			q.vertices[3].y = (int)io.DisplaySize.y - y - zoom_factor * (float)brush_size * block_size * 0.5f;

			draw_quad_line_loop(glm::vec3(1, 1, 1), (int)io.DisplaySize.x, (int)io.DisplaySize.y, 4.0, q);
		}

		else if (tool == TOOL_PAINT_CUSTOM)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			for (int i = 0; i < custom_brush1_img.cols; i++)
			{
				for (int j = 0; j < custom_brush1_img.rows; j++)
				{
					unsigned char colour = 0;

					if (custom_brush1_img.channels() == 4)
					{
						Vec<unsigned char, 4> p = custom_brush1_img.at<Vec<unsigned char, 4>>(j, i);
						colour = p[0];
					}
					else if (custom_brush1_img.channels() == 3)
					{
						Vec<unsigned char, 3> p = custom_brush1_img.at<Vec<unsigned char, 3>>(j, i);
						colour = p[0];
					}




					if (colour != 255)
						continue;



					quad q;

					float half_width = -custom_brush1_img.cols * block_size / 2.0f;
					float half_height = custom_brush1_img.rows * block_size / 2.0f;

					q.vertices[0].x = x + block_size * zoom_factor * i - block_size * 0.5f * zoom_factor;// custom_brush1_img.rows;
					q.vertices[0].y = io.DisplaySize.y - y - block_size * zoom_factor * j - block_size * 0.5f * zoom_factor;//custom_brush1_img.cols;
					q.vertices[1].x = x + block_size * zoom_factor * i - block_size * 0.5f * zoom_factor;// custom_brush1_img.rows;
					q.vertices[1].y = io.DisplaySize.y - y - block_size * zoom_factor * j + block_size * 0.5f * zoom_factor;//custom_brush1_img.cols;
					q.vertices[2].x = x + block_size * zoom_factor * i + block_size * 0.5f * zoom_factor;// custom_brush1_img.rows;
					q.vertices[2].y = io.DisplaySize.y - y - block_size * zoom_factor * j + block_size * 0.5f * zoom_factor;//custom_brush1_img.cols;
					q.vertices[3].x = x + block_size * zoom_factor * i + block_size * 0.5f * zoom_factor;// custom_brush1_img.rows;
					q.vertices[3].y = io.DisplaySize.y - y - block_size * zoom_factor * j - block_size * 0.5f * zoom_factor;// custom_brush1_img.cols;

					q.vertices[0].x += half_width * zoom_factor;
					q.vertices[1].x += half_width * zoom_factor;
					q.vertices[2].x += half_width * zoom_factor;
					q.vertices[3].x += half_width * zoom_factor;

					q.vertices[0].y += half_height * zoom_factor;
					q.vertices[1].y += half_height * zoom_factor;
					q.vertices[2].y += half_height * zoom_factor;
					q.vertices[3].y += half_height * zoom_factor;


					draw_quad_line_loop(glm::vec3(1, 1, 1), (int)io.DisplaySize.x, (int)io.DisplaySize.y, 4.0, q);
				}
			}
		}






		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}


	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}