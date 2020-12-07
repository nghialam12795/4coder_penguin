// ==========================================================================
// A file written by:
//  _______          .__    .__         .____
//  \      \    ____ |  |__ |__|____    |    |   _____    _____
//  /   |   \  / ___\|  |  \|  \__  \   |    |   \__  \  /     \
// /    |    \/ /_/  >   Y  \  |/ __ \_ |    |___ / __ \|  Y Y  \
// \____|__  /\___  /|___|  /__(____  / |_______ (____  /__|_|  /
//    /_____/     \/      \/        \/                \/      \/
// --------------------------------------------------------------
// License: GNU GENERAL PUBLIC LICENSE Version 3
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// ==========================================================================

#if !defined(FCODER_NGHIALAM_HELPERS)
#define FCODER_NGHIALAM_HELPERS

//~ NOTE(Nghia Lam): Type definition and structures
enum WINMOVE_DIRECTION {
  WINMOVE_UP,
  WINMOVE_LEFT,
  WINMOVE_DOWN,
  WINMOVE_RIGHT
};

//~ NOTE(Nghia Lam): Main Function Helpers
function void NL_WindmoveToPanel(Application_Links* app, u8 direction, b32 swap_on_move);
function void NL_CursorInterpolation(Application_Links *app, Frame_Info frame_info, Rect_f32 *rect, Rect_f32 *last_rect, Rect_f32 target);

//~ NOTE(Nghia Lam): My Implementation
function void NL_WindmoveToPanel(Application_Links* app, 
                                 u8 direction, 
                                 b32 swap_on_move) {
  View_ID view_id      = get_active_view(app, Access_Always);
  Buffer_ID cur_buffer = view_get_buffer(app, view_id, Access_Always);
  
  Panel_ID original_panel = view_get_panel(app, view_id);
  Rect_f32 original_rect  = view_get_screen_rect(app, view_id);
  
  Panel_ID current_panel = original_panel;
  
  Side preferred_side        = (direction == WINMOVE_UP   || direction == WINMOVE_LEFT ? Side_Min : Side_Max);
  b32 should_move_horizontal = (direction == WINMOVE_LEFT || direction == WINMOVE_RIGHT);
  
  Panel_ID move_top_node = 0;
  if(panel_get_parent(app, original_panel) != 0) {
    while(move_top_node == 0) {
      Panel_ID parent = panel_get_parent(app, current_panel);
      if(!parent) {
        break;
      }
      
      Panel_ID min_panel = panel_get_child(app, parent, Side_Min);
      Panel_ID max_panel = panel_get_child(app, parent, Side_Max);
      
      b32 is_on_wrong_side_of_split = ((current_panel == min_panel && preferred_side == Side_Min) ||
                                       (current_panel == max_panel && preferred_side == Side_Max));
      
      b32 is_wrong_split = false;
      
      if(!is_on_wrong_side_of_split) {
        Panel_ID min_search = min_panel;
        while(!panel_is_leaf(app, min_search)) {
          min_search = panel_get_child(app, min_search, Side_Min);
        }
        
        Panel_ID max_search = max_panel;
        while(!panel_is_leaf(app, max_search)) {
          max_search = panel_get_child(app, max_search, Side_Min);
        }
        
        View_ID min_search_view_id = panel_get_view(app, min_search, Access_Always);
        View_ID max_search_view_id = panel_get_view(app, max_search, Access_Always);
        
        Rect_f32 min_origin_rect = view_get_screen_rect(app, min_search_view_id);
        Rect_f32 max_origin_rect = view_get_screen_rect(app, max_search_view_id);
        
        b32 is_vertical   = (min_origin_rect.x0 != max_origin_rect.x0 && min_origin_rect.y0 == max_origin_rect.y0);
        b32 is_horizontal = (min_origin_rect.y0 != max_origin_rect.y0 && min_origin_rect.x0 == max_origin_rect.x0);
        
        if(should_move_horizontal && is_horizontal || !should_move_horizontal && is_vertical) {
          is_wrong_split = true;
        }
      }
      
      if(is_on_wrong_side_of_split || is_wrong_split) {
        current_panel = parent;
      } else {
        move_top_node = parent;
      }
    }
  }
  
  if(move_top_node != 0) {
    current_panel = panel_get_child(app, move_top_node, preferred_side);
    
    while(!panel_is_leaf(app, current_panel)) {
      Panel_ID min_panel = panel_get_child(app, current_panel, Side_Min);
      Panel_ID max_panel = panel_get_child(app, current_panel, Side_Max);
      
      Panel_ID min_search = min_panel;
      while(!panel_is_leaf(app, min_search)) {
        min_search = panel_get_child(app, min_search, Side_Min);
      }
      
      Panel_ID max_search = max_panel;
      while(!panel_is_leaf(app, max_search)) {
        max_search = panel_get_child(app, max_search, Side_Min);
      }
      
      Rect_f32 min_origin_rect = view_get_screen_rect(app, panel_get_view(app, min_search, Access_Always));
      Rect_f32 max_origin_rect = view_get_screen_rect(app, panel_get_view(app, max_search, Access_Always));
      
      b32 is_vertical   = (min_origin_rect.x0 != max_origin_rect.x0 && min_origin_rect.y0 == max_origin_rect.y0);
      b32 is_horizontal = (min_origin_rect.y0 != max_origin_rect.y0 && min_origin_rect.x0 == max_origin_rect.x0);
      
      if(!should_move_horizontal && is_horizontal || should_move_horizontal && is_vertical) {
        if(preferred_side == Side_Min) {
          current_panel = max_panel;
        } else {
          current_panel = min_panel;
        }
      } else {
        f32 dist_from_min = 0;
        f32 dist_from_max = 0;
        
        if(should_move_horizontal) {
          dist_from_min = original_rect.y0 - min_origin_rect.y0;
          dist_from_max = original_rect.y0 - max_origin_rect.y0;
        } else {
          dist_from_min = original_rect.x0 - min_origin_rect.x0;
          dist_from_max = original_rect.x0 - max_origin_rect.x0;
        }
        
        if(dist_from_max < 0 || dist_from_min < dist_from_max) {
          current_panel = min_panel;
        } else {
          current_panel = max_panel;
        }
      }
    }
    
    View_ID target_view = panel_get_view(app, current_panel, Access_Always);
    view_set_active(app, target_view);
    
    if(swap_on_move) {
      Buffer_ID target_buffer = view_get_buffer(app, target_view, Access_Always);
      view_set_buffer(app, target_view, cur_buffer, Access_Always);
      view_set_buffer(app, view_id, target_buffer, Access_Always);
    }
  }
}

function void NL_CursorInterpolation(Application_Links *app, 
                                     Frame_Info frame_info, 
                                     Rect_f32 *rect, 
                                     Rect_f32 *last_rect, 
                                     Rect_f32 target) {
  *last_rect = *rect;
  
  float x_change = target.x0 - rect->x0;
  float y_change = target.y0 - rect->y0;
  
  float cursor_size_x = (target.x1 - target.x0);
  float cursor_size_y = (target.y1 - target.y0) * (1 + fabsf(y_change) / 60.f);
  
  b32 should_animate_cursor = !global_battery_saver;
  if(should_animate_cursor) {
    if(fabs(x_change) > 1.f || fabs(y_change) > 1.f) {
      animate_in_n_milliseconds(app, 0);
    }
  }
  else {
    *rect = *last_rect = target;
    cursor_size_y = target.y1 - target.y0;
  }
  
  if(should_animate_cursor) {
    rect->x0 += (x_change) * frame_info.animation_dt * 30.f;
    rect->y0 += (y_change) * frame_info.animation_dt * 30.f;
    rect->x1 = rect->x0 + cursor_size_x;
    rect->y1 = rect->y0 + cursor_size_y;
  }
  
  if(target.y0 > last_rect->y0) {
    if(rect->y0 < last_rect->y0) {
      rect->y0 = last_rect->y0;
    }
  } else {
    if(rect->y1 > last_rect->y1) {
      rect->y1 = last_rect->y1;
    }
  }
}

#endif // FCODER_NGHIALAM_HELPERS