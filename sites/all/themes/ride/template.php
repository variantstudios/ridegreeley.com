<?php

function roubaix_menu_item($link, $has_children, $menu = '', $in_active_trail = FALSE, $extra_class = NULL) {
  $class = ($menu ? 'expanded' : ($has_children ? 'collapsed' : 'leaf'));
  if (!empty($extra_class)) {
    $class .= ' '. $extra_class;
  }
  if ($in_active_trail) {
    $class .= ' active-trail';
  }
  $id = preg_replace("/[^a-zA-Z0-9]/", "", strip_tags($link));
  return '<li id="'.$id.'" class="'. $class .'">'. $link . $menu ."</li>\n";
}

function roubaix_preprocess_node(&$vars) {
  global $user;
  
  if(count($user->roles) > 1) {
    $vars['edit'] = '<a href="/node/' . $vars['nid'] . '/edit">[edit]</a>';
  }
}