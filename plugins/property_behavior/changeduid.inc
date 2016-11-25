<?php

/**
 * @file
 * Implementation of 'Last user that changed' property behavior plugin for eck.
 */

// Plugin definition.
$plugin = array(
  'label' => 'Last user that changed',
  'entity_save' => 'eckextend_changeduid_property_entity_save',
  'property_info' => 'eckextend_changeduid_property_property_info',
);

/**
 * Implements entity_save plugin.
 */
function eckextend_changeduid_property_entity_save($property, $vars) {
  global $user;
  $vars['entity']->{$property} = $vars['entity']->uid;
}

/**
 * Implements 'property_info' plugin.
 */
function eckextend_changeduid_property_property_info($property, $vars) {
  $vars['properties'][$property]['type'] = 'user';
  return $vars;
}
