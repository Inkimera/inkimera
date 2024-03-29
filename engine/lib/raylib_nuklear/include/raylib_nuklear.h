#ifndef NUKLEAR_RAYLIB_H
#define NUKLEAR_RAYLIB_H

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
/*#define NK_INCLUDE_FONT_BAKING
 #define NK_INCLUDE_DEFAULT_FONT
 #define NK_INCLUDE_COMMAND_USERDATA
 #define NK_BUTTON_TRIGGER_ON_RELEASE
 */
#define NK_KEYSTATE_BASED_INPUT
#define STBRP_STATIC
#include "nuklear.h"
#include "raylib.h"

NK_API struct nk_context*
nk_raylib_init(
  Font font,
  float font_size
);

NK_API void
nk_raylib_deinit(
  struct nk_context *ctx
);

NK_API void
nk_window_set_focus_backgrounded(
  struct nk_context *ctx,
  const char *name
);

NK_API void
nk_raylib_draw(
  struct nk_context *ctx
);

NK_API void
nk_raylib_input(
  struct nk_context *ctx
);
#endif

#ifdef NUKLEAR_RAYLIB_IMPLEMENTATION
#ifndef NUKLEAR_RAYLIB_IMPLEMENTATION_ONCE
#define NUKLEAR_RAYLIB_IMPLEMENTATION_ONCE

#define NUKLEAR_RAYLIB_FONTSIZE 12.0
#define NUKLEAR_RAYLIB_MAX_VERT_SIZE 512 * 1024
#define NUKLEAR_RAYLIB_MAX_ELEMENT_SIZE 128 * 1024

#include "string.h"
#include "rlgl.h"

/*
 * Private
 */
typedef struct {
  float pos[2];
  float uv[2];
  nk_byte col[4];
} nk_raylib_vertex_t;

static Texture2D null_texture;
static struct nk_draw_null_texture null_tex = { 0 };
static nk_raylib_vertex_t verts[NUKLEAR_RAYLIB_MAX_VERT_SIZE];
static nk_draw_index idx[NUKLEAR_RAYLIB_MAX_ELEMENT_SIZE];
static struct nk_buffer cmds;

static float
nk_raylib_font_get_text_width(
  nk_handle handle,
  float height,
  const char *text,
  int len
);

static void
nk_raylib_font_glyph_query(
  nk_handle handle,
  float font_height,
  struct nk_user_font_glyph *glyph,
  nk_rune codepoint,
  nk_rune next_codepoint
);

static void
nk_raylib_clipboard_paste(
  nk_handle usr,
  struct nk_text_edit *edit
);

static void
nk_raylib_clipboard_copy(
  nk_handle usr,
  const char *text,
  int len
);

/*
 * Public
 */

NK_API struct nk_context*
nk_raylib_init(
  Font font,
  float font_size
) {

  struct Font *font_copy = (struct Font*)MemAlloc(sizeof(struct Font));
  if (font_size <= 0) {
    font_size = NUKLEAR_RAYLIB_FONTSIZE;
  }
  font_copy->baseSize = font.baseSize;
  font_copy->glyphCount = font.glyphCount;
  font_copy->glyphPadding = font.glyphPadding;
  font_copy->glyphs = font.glyphs;
  font_copy->recs = font.recs;
  font_copy->texture = font.texture;

  struct nk_user_font *f = (struct nk_user_font*)malloc(sizeof(struct nk_user_font));
  f->userdata = nk_handle_ptr(font_copy);
  f->texture.id = font_copy->texture.id;
  f->height = font_size;
  f->width = &nk_raylib_font_get_text_width;
  f->query = &nk_raylib_font_glyph_query;

  struct nk_context *ctx = (struct nk_context*)malloc(sizeof(struct nk_context));
  nk_init_default(ctx, f);
  nk_style_set_font(ctx, f);
  ctx->clip.copy = nk_raylib_clipboard_copy;
  ctx->clip.paste = nk_raylib_clipboard_paste;
  ctx->clip.userdata = nk_handle_ptr(0);

  Image null_img = GenImageColor(1, 1, WHITE);
  null_texture = LoadTextureFromImage(null_img);
  null_tex.texture.id = null_texture.id;
  null_tex.uv = nk_vec2(0.5, 0.5);
  nk_buffer_init_default(&cmds);
  return ctx;
}

NK_API void
nk_raylib_deinit(
  struct nk_context *ctx
) {
  struct nk_user_font *f = (struct nk_user_font*)ctx->style.font;
  if (f != NULL) {
    void *fptr = f->userdata.ptr;
    if (fptr != NULL) {
      free(fptr);
    }
    free(f);
    ctx->style.font = NULL;
  }
  UnloadTexture(null_texture);
  nk_buffer_free(&cmds);
  nk_free(ctx);
}

/*
 * https://github.com/Immediate-Mode-UI/Nuklear/issues/470
 */
/*
   NK_API void
   nk_window_set_focus_backgrounded(
   struct nk_context *ctx,
   const char *name
   ) {
   int title_len;
   nk_hash title_hash;
   struct nk_window *win;
   NK_ASSERT(ctx);
   if (!ctx) return;
   title_len = (int)nk_strlen(name);
   title_hash = nk_murmur_hash(name, (int)title_len, NK_WINDOW_TITLE);
   win = nk_find_window(ctx, title_hash, name);
   if (win && ctx->end != win) {
    nk_remove_window(ctx, win);
    if (win->flags & NK_WINDOW_BACKGROUND) {
      nk_insert_window(ctx, win, NK_INSERT_FRONT);
    } else {
      nk_insert_window(ctx, win, NK_INSERT_BACK);
    }
   }
   ctx->active = win;
   }
 */

static float
nk_raylib_font_get_text_width(
  nk_handle handle,
  float font_height,
  const char *text,
  int len
) {
  Font *font = handle.ptr;
  if (!font) {
    return 0.0;
  }
  const char *subtext = TextSubtext(text, 0, len);
  float scale_factor = font_height / font->baseSize;
  float text_size = 0.0;
  float glyph_width = 0.0;
  for (int i = 0, codepoint_size = 0; i < len; i += codepoint_size) {
    int codepoint = GetCodepointNext(&subtext[i], &codepoint_size);
    GlyphInfo info = GetGlyphInfo(*font, codepoint);
    Rectangle rect = GetGlyphAtlasRec(*font, codepoint);
    if (info.advanceX == 0) {
      glyph_width = (rect.width * scale_factor);
    }   else{
      glyph_width = (info.advanceX * scale_factor);
    }
    text_size += glyph_width;
  }
  return text_size;
}

static void
nk_raylib_font_glyph_query(
  nk_handle handle,
  float font_height,
  struct nk_user_font_glyph *glyph,
  nk_rune codepoint,
  nk_rune next_codepoint
) {
  if (!handle.ptr) {
    return;
  }
  Font *font = handle.ptr;
  float width = (float)font->texture.width;
  float height = (float)font->texture.height;
  GlyphInfo info = GetGlyphInfo(*font, codepoint);
  Rectangle rect = GetGlyphAtlasRec(*font, codepoint);
  float scale_factor = font_height / font->baseSize;
  Rectangle src = {
    rect.x - (float)font->glyphPadding,
    rect.y - (float)font->glyphPadding,
    rect.width + 2.0f * font->glyphPadding,
    rect.height + 2.0f * font->glyphPadding
  };
  glyph->width = (rect.width + 2.0f * font->glyphPadding) * scale_factor;
  glyph->height = (rect.height + 2.0f * font->glyphPadding) * scale_factor;
  if (info.advanceX == 0) {
    glyph->xadvance = rect.width * scale_factor;
  } else {
    glyph->xadvance = info.advanceX * scale_factor;
  }
  glyph->uv[0].x = src.x / width;
  glyph->uv[0].y = src.y / height;
  glyph->uv[1].x = (src.x + src.width) / width;
  glyph->uv[1].y = (src.y + src.height) / height;

  glyph->offset.x = info.offsetX * scale_factor - font->glyphPadding * scale_factor;
  glyph->offset.y = info.offsetY * scale_factor - font->glyphPadding * scale_factor;
}

#define FOR_ALL_KEYS(X) \
  X(KEY_APOSTROPHE); \
  X(KEY_QUESTION); \
  X(KEY_COMMA); \
  X(KEY_MINUS); \
  X(KEY_PERIOD); \
  X(KEY_SLASH); \
  X(KEY_ZERO); \
  X(KEY_ONE); \
  X(KEY_TWO); \
  X(KEY_THREE); \
  X(KEY_FOUR); \
  X(KEY_FIVE); \
  X(KEY_SIX); \
  X(KEY_SEVEN); \
  X(KEY_EIGHT); \
  X(KEY_NINE); \
  X(KEY_SEMICOLON); \
  X(KEY_EQUAL); \
  X(KEY_A); \
  X(KEY_B); \
  X(KEY_C); \
  X(KEY_D); \
  X(KEY_E); \
  X(KEY_F); \
  X(KEY_G); \
  X(KEY_H); \
  X(KEY_I); \
  X(KEY_J); \
  X(KEY_K); \
  X(KEY_L); \
  X(KEY_M); \
  X(KEY_N); \
  X(KEY_O); \
  X(KEY_P); \
  X(KEY_Q); \
  X(KEY_R); \
  X(KEY_S); \
  X(KEY_T); \
  X(KEY_U); \
  X(KEY_V); \
  X(KEY_W); \
  X(KEY_X); \
  X(KEY_Y); \
  X(KEY_Z); \
  X(KEY_SPACE); \
  X(KEY_ESCAPE); \
  X(KEY_ENTER); \
  X(KEY_TAB); \
  X(KEY_BACKSPACE); \
  X(KEY_INSERT); \
  X(KEY_DELETE); \
  X(KEY_RIGHT); \
  X(KEY_LEFT); \
  X(KEY_DOWN); \
  X(KEY_UP); \
  X(KEY_PAGE_UP); \
  X(KEY_PAGE_DOWN); \
  X(KEY_HOME); \
  X(KEY_END); \
  X(KEY_CAPS_LOCK); \
  X(KEY_SCROLL_LOCK); \
  X(KEY_NUM_LOCK); \
  X(KEY_PRINT_SCREEN); \
  X(KEY_PAUSE); \
  X(KEY_F1); \
  X(KEY_F2); \
  X(KEY_F3); \
  X(KEY_F4); \
  X(KEY_F5); \
  X(KEY_F6); \
  X(KEY_F7); \
  X(KEY_F8); \
  X(KEY_F9); \
  X(KEY_F10); \
  X(KEY_F11); \
  X(KEY_F12); \
  X(KEY_LEFT_SHIFT); \
  X(KEY_LEFT_CONTROL); \
  X(KEY_LEFT_ALT); \
  X(KEY_LEFT_SUPER); \
  X(KEY_RIGHT_SHIFT); \
  X(KEY_RIGHT_CONTROL); \
  X(KEY_RIGHT_ALT); \
  X(KEY_RIGHT_SUPER); \
  X(KEY_KB_MENU); \
  X(KEY_LEFT_BRACKET); \
  X(KEY_BACKSLASH); \
  X(KEY_RIGHT_BRACKET); \
  X(KEY_GRAVE); \
  X(KEY_KP_0); \
  X(KEY_KP_1); \
  X(KEY_KP_2); \
  X(KEY_KP_3); \
  X(KEY_KP_4); \
  X(KEY_KP_5); \
  X(KEY_KP_6); \
  X(KEY_KP_7); \
  X(KEY_KP_8); \
  X(KEY_KP_9); \
  X(KEY_KP_DECIMAL); \
  X(KEY_KP_DIVIDE); \
  X(KEY_KP_MULTIPLY); \
  X(KEY_KP_SUBTRACT); \
  X(KEY_KP_ADD); \
  X(KEY_KP_ENTER); \
  X(KEY_KP_EQUAL);

#define SET_KEY_DOWN(ctx, KEY) nk_input_key(ctx, ) = IsKeyDown(KEY)

static void
nk_raylib_input_keyboard(
  struct nk_context *ctx
) {
  bool control = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
  bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
  nk_input_key(ctx, NK_KEY_SHIFT, shift);
  nk_input_key(ctx, NK_KEY_CTRL, control);
  nk_input_key(ctx, NK_KEY_DEL, IsKeyDown(KEY_DELETE));
  nk_input_key(ctx, NK_KEY_ENTER, IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_KP_ENTER));
  nk_input_key(ctx, NK_KEY_TAB, IsKeyDown(KEY_TAB));
  nk_input_key(ctx, NK_KEY_BACKSPACE, IsKeyDown(KEY_BACKSPACE));
  nk_input_key(ctx, NK_KEY_COPY, IsKeyPressed(KEY_C) && control);
  nk_input_key(ctx, NK_KEY_CUT, IsKeyPressed(KEY_X) && control);
  nk_input_key(ctx, NK_KEY_PASTE, IsKeyPressed(KEY_V) && control);
  nk_input_key(ctx, NK_KEY_TEXT_LINE_START, IsKeyPressed(KEY_B) && control);
  nk_input_key(ctx, NK_KEY_TEXT_LINE_END, IsKeyPressed(KEY_E) && control);
  nk_input_key(ctx, NK_KEY_TEXT_UNDO, IsKeyDown(KEY_Z) && control);
  nk_input_key(ctx, NK_KEY_TEXT_REDO, IsKeyDown(KEY_R) && control);
  nk_input_key(ctx, NK_KEY_TEXT_SELECT_ALL, IsKeyDown(KEY_A) && control);
  nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, IsKeyDown(KEY_LEFT) && control);
  nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, IsKeyDown(KEY_RIGHT) && control);
  nk_input_key(ctx, NK_KEY_LEFT, IsKeyDown(KEY_LEFT) && !control);
  nk_input_key(ctx, NK_KEY_RIGHT, IsKeyDown(KEY_RIGHT) && !control);
  //nk_input_key(ctx, NK_KEY_TEXT_INSERT_MODE, IsKeyDown());
  //nk_input_key(ctx, NK_KEY_TEXT_REPLACE_MODE, IsKeyDown());
  //nk_input_key(ctx, NK_KEY_TEXT_RESET_MODE, IsKeyDown());
  nk_input_key(ctx, NK_KEY_UP, IsKeyDown(KEY_UP));
  nk_input_key(ctx, NK_KEY_DOWN, IsKeyDown(KEY_DOWN));
  nk_input_key(ctx, NK_KEY_TEXT_START, IsKeyDown(KEY_HOME));
  nk_input_key(ctx, NK_KEY_TEXT_END, IsKeyDown(KEY_END));
  nk_input_key(ctx, NK_KEY_SCROLL_START, IsKeyDown(KEY_HOME) && control);
  nk_input_key(ctx, NK_KEY_SCROLL_END, IsKeyDown(KEY_END) && control);
  nk_input_key(ctx, NK_KEY_SCROLL_DOWN, IsKeyDown(KEY_PAGE_DOWN));
  nk_input_key(ctx, NK_KEY_SCROLL_UP, IsKeyDown(KEY_PAGE_UP));
  // Keys
  if (IsKeyPressed(KEY_APOSTROPHE)) nk_input_unicode(ctx, shift ? 34 : (nk_rune)KEY_APOSTROPHE);
  if (IsKeyPressed(KEY_COMMA)) nk_input_unicode(ctx, shift ? 60 : (nk_rune)KEY_COMMA);
  if (IsKeyPressed(KEY_MINUS)) nk_input_unicode(ctx, shift ? 95 : (nk_rune)KEY_MINUS);
  if (IsKeyPressed(KEY_PERIOD)) nk_input_unicode(ctx, shift ? 62 : (nk_rune)KEY_PERIOD);
  if (IsKeyPressed(KEY_SLASH)) nk_input_unicode(ctx, shift ? 63 : (nk_rune)KEY_SLASH);
  if (IsKeyPressed(KEY_ZERO)) nk_input_unicode(ctx, shift ? 41 : (nk_rune)KEY_ZERO);
  if (IsKeyPressed(KEY_ONE)) nk_input_unicode(ctx, shift ? 33 : (nk_rune)KEY_ONE);
  if (IsKeyPressed(KEY_TWO)) nk_input_unicode(ctx, shift ? 64 : (nk_rune)KEY_TWO);
  if (IsKeyPressed(KEY_THREE)) nk_input_unicode(ctx, shift ? 35 : (nk_rune)KEY_THREE);
  if (IsKeyPressed(KEY_FOUR)) nk_input_unicode(ctx, shift ? 36 : (nk_rune)KEY_FOUR);
  if (IsKeyPressed(KEY_FIVE)) nk_input_unicode(ctx, shift ? 37 : (nk_rune)KEY_FIVE);
  if (IsKeyPressed(KEY_SIX)) nk_input_unicode(ctx, shift ? 94 : (nk_rune)KEY_SIX);
  if (IsKeyPressed(KEY_SEVEN)) nk_input_unicode(ctx, shift ? 38 : (nk_rune)KEY_SEVEN);
  if (IsKeyPressed(KEY_EIGHT)) nk_input_unicode(ctx, shift ? 42 : (nk_rune)KEY_EIGHT);
  if (IsKeyPressed(KEY_NINE)) nk_input_unicode(ctx, shift ? 40 : (nk_rune)KEY_NINE);
  if (IsKeyPressed(KEY_SEMICOLON)) nk_input_unicode(ctx, shift ? 41 : (nk_rune)KEY_SEMICOLON);
  if (IsKeyPressed(KEY_EQUAL)) nk_input_unicode(ctx, shift ? 43 : (nk_rune)KEY_EQUAL);
  if (IsKeyPressed(KEY_A)) nk_input_unicode(ctx, shift ? KEY_A : KEY_A + 32);
  if (IsKeyPressed(KEY_B)) nk_input_unicode(ctx, shift ? KEY_B : KEY_B + 32);
  if (IsKeyPressed(KEY_C)) nk_input_unicode(ctx, shift ? KEY_C : KEY_C + 32);
  if (IsKeyPressed(KEY_D)) nk_input_unicode(ctx, shift ? KEY_D : KEY_D + 32);
  if (IsKeyPressed(KEY_E)) nk_input_unicode(ctx, shift ? KEY_E : KEY_E + 32);
  if (IsKeyPressed(KEY_F)) nk_input_unicode(ctx, shift ? KEY_F : KEY_F + 32);
  if (IsKeyPressed(KEY_G)) nk_input_unicode(ctx, shift ? KEY_G : KEY_G + 32);
  if (IsKeyPressed(KEY_H)) nk_input_unicode(ctx, shift ? KEY_H : KEY_H + 32);
  if (IsKeyPressed(KEY_I)) nk_input_unicode(ctx, shift ? KEY_I : KEY_I + 32);
  if (IsKeyPressed(KEY_J)) nk_input_unicode(ctx, shift ? KEY_J : KEY_J + 32);
  if (IsKeyPressed(KEY_K)) nk_input_unicode(ctx, shift ? KEY_K : KEY_K + 32);
  if (IsKeyPressed(KEY_L)) nk_input_unicode(ctx, shift ? KEY_L : KEY_L + 32);
  if (IsKeyPressed(KEY_M)) nk_input_unicode(ctx, shift ? KEY_M : KEY_M + 32);
  if (IsKeyPressed(KEY_N)) nk_input_unicode(ctx, shift ? KEY_N : KEY_N + 32);
  if (IsKeyPressed(KEY_O)) nk_input_unicode(ctx, shift ? KEY_O : KEY_O + 32);
  if (IsKeyPressed(KEY_P)) nk_input_unicode(ctx, shift ? KEY_P : KEY_P + 32);
  if (IsKeyPressed(KEY_Q)) nk_input_unicode(ctx, shift ? KEY_Q : KEY_Q + 32);
  if (IsKeyPressed(KEY_R)) nk_input_unicode(ctx, shift ? KEY_R : KEY_R + 32);
  if (IsKeyPressed(KEY_S)) nk_input_unicode(ctx, shift ? KEY_S : KEY_S + 32);
  if (IsKeyPressed(KEY_T)) nk_input_unicode(ctx, shift ? KEY_T : KEY_T + 32);
  if (IsKeyPressed(KEY_U)) nk_input_unicode(ctx, shift ? KEY_U : KEY_U + 32);
  if (IsKeyPressed(KEY_V)) nk_input_unicode(ctx, shift ? KEY_V : KEY_V + 32);
  if (IsKeyPressed(KEY_W)) nk_input_unicode(ctx, shift ? KEY_W : KEY_W + 32);
  if (IsKeyPressed(KEY_X)) nk_input_unicode(ctx, shift ? KEY_X : KEY_X + 32);
  if (IsKeyPressed(KEY_Y)) nk_input_unicode(ctx, shift ? KEY_Y : KEY_Y + 32);
  if (IsKeyPressed(KEY_Z)) nk_input_unicode(ctx, shift ? KEY_Z : KEY_Z + 32);
  if (IsKeyPressed(KEY_LEFT_BRACKET)) nk_input_unicode(ctx, shift ? 123 : (nk_rune)KEY_LEFT_BRACKET);
  if (IsKeyPressed(KEY_BACKSLASH)) nk_input_unicode(ctx, shift ? 124 : (nk_rune)KEY_BACKSLASH);
  if (IsKeyPressed(KEY_RIGHT_BRACKET)) nk_input_unicode(ctx, shift ? 125 : (nk_rune)KEY_RIGHT_BRACKET);
  if (IsKeyPressed(KEY_GRAVE)) nk_input_unicode(ctx, shift ? 126 : (nk_rune)KEY_GRAVE);
  // Functions
  if (IsKeyPressed(KEY_SPACE)) nk_input_unicode(ctx, KEY_SPACE);
  if (IsKeyPressed(KEY_TAB)) nk_input_unicode(ctx, 9);
  // Keypad
  if (IsKeyPressed(KEY_KP_0)) nk_input_unicode(ctx, KEY_ZERO);
  if (IsKeyPressed(KEY_KP_1)) nk_input_unicode(ctx, KEY_ONE);
  if (IsKeyPressed(KEY_KP_2)) nk_input_unicode(ctx, KEY_TWO);
  if (IsKeyPressed(KEY_KP_3)) nk_input_unicode(ctx, KEY_THREE);
  if (IsKeyPressed(KEY_KP_4)) nk_input_unicode(ctx, KEY_FOUR);
  if (IsKeyPressed(KEY_KP_5)) nk_input_unicode(ctx, KEY_FIVE);
  if (IsKeyPressed(KEY_KP_6)) nk_input_unicode(ctx, KEY_SIX);
  if (IsKeyPressed(KEY_KP_7)) nk_input_unicode(ctx, KEY_SEVEN);
  if (IsKeyPressed(KEY_KP_8)) nk_input_unicode(ctx, KEY_EIGHT);
  if (IsKeyPressed(KEY_KP_9)) nk_input_unicode(ctx, KEY_NINE);
  if (IsKeyPressed(KEY_KP_DECIMAL)) nk_input_unicode(ctx, KEY_PERIOD);
  if (IsKeyPressed(KEY_KP_DIVIDE)) nk_input_unicode(ctx, KEY_SLASH);
  if (IsKeyPressed(KEY_KP_MULTIPLY)) nk_input_unicode(ctx, 48);
  if (IsKeyPressed(KEY_KP_SUBTRACT)) nk_input_unicode(ctx, 45);
  if (IsKeyPressed(KEY_KP_ADD)) nk_input_unicode(ctx, 43);
}

static void
nk_raylib_input_mouse(
  struct nk_context *ctx
) {
  const int mouseX = GetMouseX();
  const int mouseY = GetMouseY();

  nk_input_motion(ctx, mouseX, mouseY);
  nk_input_button(ctx, NK_BUTTON_LEFT, mouseX, mouseY, IsMouseButtonDown(MOUSE_LEFT_BUTTON));
  nk_input_button(ctx, NK_BUTTON_RIGHT, mouseX, mouseY, IsMouseButtonDown(MOUSE_RIGHT_BUTTON));
  nk_input_button(ctx, NK_BUTTON_MIDDLE, mouseX, mouseY, IsMouseButtonDown(MOUSE_MIDDLE_BUTTON));

  float mouseWheel = GetMouseWheelMove();
  if (mouseWheel != 0.0f) {
    struct nk_vec2 mouseWheelMove;
    mouseWheelMove.x = 0.0f;
    mouseWheelMove.y = mouseWheel;
    nk_input_scroll(ctx, mouseWheelMove);
  }
}

NK_API void
nk_raylib_input(
  struct nk_context *ctx
) {
  ctx->delta_time_seconds = GetFrameTime();
  nk_input_begin(ctx);
  {
    nk_raylib_input_keyboard(ctx);
    nk_raylib_input_mouse(ctx);
  }
  nk_input_end(ctx);
}

static void
nk_raylib_clipboard_paste(
  nk_handle usr,
  struct nk_text_edit *edit
) {
  const char *text = GetClipboardText();
  NK_UNUSED(usr);
  if (text != NULL) {
    nk_textedit_paste(edit, text, (int)TextLength(text));
  }
}

static void
nk_raylib_clipboard_copy(
  nk_handle usr,
  const char *text,
  int len
) {
  NK_UNUSED(usr);
  NK_UNUSED(len);
  SetClipboardText(text);
}

/*
   static void
   ImGui_ImplRaylib_UpdateMouseCursor() {
   struct ImGuiIO *io = igGetIO();
   if (io->ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
    return;
   ImGuiMouseCursor imgui_cursor = igGetMouseCursor();
   if (io->MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None) {
    // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
    HideCursor();
   }else {
    // Show OS mouse cursor
    ShowCursor();
   }
   }

   static void
   ImGui_ImplRaylib_UpdateMousePosAndButtons() {
   struct ImGuiIO *io = igGetIO();
   // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
   if (io->WantSetMousePos)
    SetMousePosition(io->MousePos.x, io->MousePos.y);
   else
    io->MousePos = (ImVec2) { -FLT_MAX, -FLT_MAX };

   io->MouseDown[0] = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
   io->MouseDown[1] = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
   io->MouseDown[2] = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);
   if (!IsWindowMinimized()) {
    io->MousePos = (ImVec2) { GetTouchX(), GetTouchY() };
   }
   }
 */

/*
   void
   ImGui_ImplRaylib_NewFrame() {
   struct ImGuiIO *io = igGetIO();

   io->DisplaySize = (ImVec2) { (float) GetScreenWidth(), (float) GetScreenHeight() };

   double current_time = GetTime();
   io->DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
   g_Time = current_time;

   io->KeyCtrl = IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
   io->KeyShift = IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT);
   io->KeyAlt = IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT);
   io->KeySuper = IsKeyDown(KEY_RIGHT_SUPER) || IsKeyDown(KEY_LEFT_SUPER);

   ImGui_ImplRaylib_UpdateMousePosAndButtons();
   ImGui_ImplRaylib_UpdateMouseCursor();
   if (GetMouseWheelMove() > 0)
    io->MouseWheel += 1;
   else if (GetMouseWheelMove() < 0)
    io->MouseWheel -= 1;
   }

 #define SET_KEY_DOWN(KEY) io->KeysDown[KEY] = IsKeyDown(KEY)

   bool
   ImGui_ImplRaylib_ProcessEvent() {
   struct ImGuiIO *io = igGetIO();

   FOR_ALL_KEYS(SET_KEY_DOWN);

   int keyPressed = GetKeyPressed();
   if (keyPressed > 0) {
    ImGuiIO_AddInputCharacter(io, keyPressed);
   }
   return true;
   }
 */

static void
nk_raylib_draw_triangle_vertex(
  nk_raylib_vertex_t vert
) {
  rlColor4ub(vert.col[0], vert.col[1], vert.col[2], vert.col[3]);
  rlNormal3f(0.0f, 0.0f, 1.0f);
  rlTexCoord2f(vert.uv[0], vert.uv[1]);
  rlVertex2f(vert.pos[0], vert.pos[1]);
}

static void
nk_raylib_draw_triangles(
  struct nk_context *ctx,
  const struct nk_draw_command *cmd,
  const nk_raylib_vertex_t *verts,
  const nk_draw_index *idx,
  int offset
) {
  for (int i = 0; i < cmd->elem_count; i += 3) {
    nk_draw_index index;
    nk_raylib_vertex_t vertex;

    index = idx[i + offset];
    vertex = verts[index];
    nk_raylib_draw_triangle_vertex(vertex);

    index = idx[i + offset + 2];
    vertex = verts[index];
    nk_raylib_draw_triangle_vertex(vertex);

    index = idx[i + offset + 1];
    vertex = verts[index];
    nk_raylib_draw_triangle_vertex(vertex);

  }
}

NK_API void
nk_raylib_draw(
  struct nk_context *ctx
) {
  struct nk_buffer vbuf, ebuf;
  //const nk_raylib_vertex_t *verts;
  //const nk_draw_index *idx;
  //nk_buffer_init_default(&cmds);
  //nk_buffer_init_default(&vbuf);
  //nk_buffer_init_default(&ebuf);
  nk_buffer_init_fixed(&vbuf, (void*)verts, NUKLEAR_RAYLIB_MAX_VERT_SIZE);
  nk_buffer_init_fixed(&ebuf, (void*)idx, NUKLEAR_RAYLIB_MAX_ELEMENT_SIZE);

  static const struct nk_draw_vertex_layout_element vertex_layout[] = {
    { NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(nk_raylib_vertex_t, pos) },
    { NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(nk_raylib_vertex_t, uv) },
    { NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(nk_raylib_vertex_t, col) },
    { NK_VERTEX_LAYOUT_END }
  };
  struct nk_convert_config cfg;
  memset(&cfg, 0, sizeof(cfg));
  cfg.vertex_layout = vertex_layout;
  cfg.vertex_size = sizeof(nk_raylib_vertex_t);
  cfg.vertex_alignment = NK_ALIGNOF(nk_raylib_vertex_t);
  cfg.tex_null = null_tex;
  cfg.circle_segment_count = 22;
  cfg.curve_segment_count = 22;
  cfg.arc_segment_count = 22;
  cfg.global_alpha = 1.0;
  cfg.shape_AA = NK_ANTI_ALIASING_ON;
  cfg.line_AA = NK_ANTI_ALIASING_ON;

  nk_flags flags = nk_convert(ctx, &cmds, &vbuf, &ebuf, &cfg);
  if (flags != NK_CONVERT_SUCCESS) {
    printf("nk_raylib_draw convert failed\n");
    return;
  }
  //verts = (const nk_raylib_vertex_t*)nk_buffer_memory_const(&vbuf);
  //idx = (const nk_draw_index*)nk_buffer_memory_const(&ebuf);

  rlDisableDepthTest();
  rlDisableBackfaceCulling();
  BeginBlendMode(BLEND_ALPHA);
  const struct nk_draw_command *cmd;
  int offset = 0;
  nk_draw_foreach(cmd, ctx, &cmds) {
    if (!cmd->elem_count) {
      continue;
    }
    int rect_x = (int)(cmd->clip_rect.x);
    int rect_y = (int)(cmd->clip_rect.y);
    int rect_w = (int)(cmd->clip_rect.w);
    int rect_h = (int)(cmd->clip_rect.h);
    BeginScissorMode(rect_x, rect_y, rect_w, rect_h);
    rlPushMatrix();
    rlBegin(RL_TRIANGLES);
    rlSetTexture(cmd->texture.id);
    nk_raylib_draw_triangles(ctx, cmd, verts, idx, offset);
    offset += cmd->elem_count;
    rlSetTexture(0);
    rlEnd();
    rlPopMatrix();
  }
  EndScissorMode();
  rlEnableDepthTest();
  rlEnableBackfaceCulling();
  EndBlendMode();
  nk_buffer_clear(&cmds);
  nk_clear(ctx);
}
#endif
#endif
