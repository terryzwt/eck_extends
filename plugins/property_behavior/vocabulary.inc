<?php

/**
 * @file
 * Implementation of 'Vocabulary' property behavior plugin for eck.
 */


// @TODO: copy functionality from Reference.

// Plugin definition.
$plugin = array(
  'label' => 'Vocabulary',
  'default_widget' => 'eckextend_vocabulary_property_widget',
  'default_formatter' => 'eckextend_vocabulary_property_formatter',
  'property_info' => 'eckextend_vocabulary_property_info',
);

/**
 * Implements 'property_info' plugin.
 */
function eckextend_vocabulary_property_info($property, $vars) {
  // Validate the vocabulary format (id_{vocabulary}) type.
  if ($entity_name = eckextend_reference_get_entity_name($property)) {
    $vars['properties'][$property]['type'] = 'taxonomy_term';
    $vars['properties'][$property]['description'] = t('Vocabulary @vocab reference.',
      array('@vocab' => ucfirst($entity_name)));
  }
  return $vars;
}

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_vocabulary_property_widget($property, $vars) {
  $entity = $vars['entity'];
  $value_raw = _eckextend_vocabulary_extract($entity, $property, TRUE);
  $entity_name = eckextend_reference_get_entity_name($property);

  // CRUD form element.
  $element = array(
    '#type' => 'entityreference',
    '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
    // Module entityreference_autocomplete form specific.
    '#era_entity_type' => 'taxonomy_term',
    '#era_bundles' => array($entity_name),
    '#default_value' => ($value_raw) ? $value_raw : '',
    '#required' => eckextend_get_property_required_info($entity, $property),
    '#access' => eckextend_property_access($entity, $property, 'edit'),
    // Custom validation function are included in the module file.
    '#element_validate' => array('eckextend_vocabulary_property_validate'),
  );

  return $element;
}

/**
 * Implements 'default_formatter' plugin.
 */
function eckextend_vocabulary_property_formatter($property, $vars) {
  $entity = $vars['entity'];
  $value = _eckextend_vocabulary_extract($entity, $property);
  return eckextend_theme_property($value, $property, $entity, 'vocabulary');
}

/**
 * Helper function to extract property value.
 */
function _eckextend_vocabulary_extract($entity, $property, $raw = FALSE) {
  $value = 0;
  if (isset($entity->{$property})) {
    if ($raw) {
      $value = $entity->{$property};
    }
    else {
      // Get the taxonomy term name and return it.
      if ($term = taxonomy_term_load($entity->{$property})) {
        // Let's get this translated.
        if (module_exists('i18n_taxonomy')) {
          $translated_term = i18n_taxonomy_localize_terms($term);

          return $translated_term->name;
        }
        else {
          return $term->name;
        }
      }
    }
  }

  return $value;
}
