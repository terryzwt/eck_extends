<?php

/**
 * @file
 * Implementation of 'OG Group ID' property behavior plugin for eck.
 */

if (module_exists('og')) {
  $plugin = array(
    'label' => 'OG Group ID',
    'entity_save' => 'eckextend_gid_property_entity_save',
    'property_info' => 'eckextend_gid_property_property_info',
  );
}

/**
 * Implements 'entity_save' plugin.
 */
function eckextend_gid_property_entity_save($property, $vars) {
  // It assumes the entity uses 'og_group_ref' field name.
  // @TODO: find the OG field and get it's value.
  $entity = $vars['entity'];
  if (isset($entity->og_group_ref[LANGUAGE_NONE][0]['target_id'])) {
    $entity->{$property} = $entity->og_group_ref[LANGUAGE_NONE][0]['target_id'];
  }
  else {
    $entity->{$property} = NULL;
  }
}

/**
 * Implements 'property_info' plugin.
 */
function eckextend_gid_property_property_info($property, $vars) {
  // This property is OG module dependent.
  $vars['properties'][$property]['type'] = 'og_membership';
  $vars['properties'][$property]['description'] = 'OG Group ID';
  return $vars;
}
