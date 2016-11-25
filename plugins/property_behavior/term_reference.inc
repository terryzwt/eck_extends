<?php

/**
 * @file
 * Implementation of 'Reference' property behavior plugin for eck.
 */

if (module_exists('taxonomy')) {
  $plugin = array(
    'label' => 'Term Reference (field replica)',
    'default_widget' => 'eckextend_term_reference_property_widget',
    'default_formatter' => 'eckextend_term_reference_property_formatter',
    'property_form' => 'eckextend_term_reference_property_form',
    'property_info' => 'eckextend_term_reference_property_info',
  );
}

/**
 * Implements 'property_info' plugin.
 */
function eckextend_term_reference_property_info($property, $vars) {
  $vars['properties'][$property]['type'] = 'taxonomy_term';
  return $vars;
}

/**
 * Provide plugin specific form elements.
 */
function eckextend_term_reference_property_form($property, $vars) {
  $settings = isset($vars['settings']) ? $vars['settings'] : NULL;

  // Property specific form.
  $form = eckextend_property_default_form($property, $vars);

  $form['type'] = array(
    '#title' => t('Widget Type'),
    '#type' => 'select',
    '#description' => t('The type of form element you would like to present to the user when creating this property.'),
    '#options' => array(
      'select' => t('Select list'),
      'radios' => t('Radios'),
      'taxonomy_autocomplete' => t('Autocomplete term widget (tagging)'),
    ),
    '#default_value' => isset($settings['type']) ? $settings['type'] : NULL,
  );
  if (module_exists('entityreference_autocomplete')) {
    $form['type']['#options']['entityreference'] = t('Entityreference autocomplete');
  }
  $vocabularies = array();
  foreach (taxonomy_get_vocabularies() as $vocabulary) {
    $vocabularies[$vocabulary->machine_name] = $vocabulary->name;
  }
  $form['vocabulary'] = array(
    '#type' => 'select',
    '#title' => t('Vocabulary'),
    '#options' => $vocabularies,
    '#description' => t('The vocabulary which supplies the options for this field.'),
    '#default_value' => isset($settings['vocabulary']) ? $settings['vocabulary'] : NULL,
    '#required' => TRUE,
  );

  return $form;
}

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_term_reference_property_widget($property, $vars) {
  if ($settings = eckextend_property_settings($vars['entity']->entityType(), $property)) {
    $entity = $vars['entity'];
    $element = array(
      '#type' => $settings['type'],
      '#required' => $settings['required'],
      '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
      '#access' => eckextend_property_access($entity, $property, 'edit'),
      '#default_value' => eckextend_get_property_value($entity, $property),
      '#element_validate' => array('eckextend_null_property'),
    );
    $bundles = array($settings['vocabulary']);
    switch ($settings['type']) {
      // Entityreference autocomplete specific.
      case 'entityreference':
        $element['#era_entity_type'] = 'taxonomy_term';
        $element['#era_bundles'] = array($settings['vocabulary']);
        // We can use the same validate function as reference behavior.
        $element['#element_validate'] = array('_eckextend_reference_entityreference_validate');
        break;

      // Add options checkboxes.
      case 'radios':
        $entity_list = _eckextend_reference_entity_list('taxonomy_term', array($settings['vocabulary']));
        if (!$settings['required']) {
          $entity_list = array(NULL => t('N/A')) + $entity_list;
        }
        $element['#options'] = $entity_list;
        break;

      // Add options select.
      case 'select':
        if (!$settings['required']) {
          $element['#empty_option'] = t('- None -');
        }
        else {
          $element['#empty_option'] = t('- Select a value -');
        }
        $element['#options'] = _eckextend_reference_entity_list('taxonomy_term', array($settings['vocabulary']));
        break;

      // Default.
      default:
        return;
    }

    return $element;
  }
}

/**
 * Implements 'default_formatter' plugin.
 */
function eckextend_term_reference_property_formatter($property, $vars) {
  // Get property settings.
  if ($settings = eckextend_property_settings($vars['entity']->entityType(), $property)) {
    // Get property value, in this case an entity ID.
    if ($entity_id = eckextend_get_property_value($vars['entity'], $property)) {
      // Load the entity and check for entity view access.
      $entity = entity_load_single('taxonomy_term', $entity_id);
      if (entity_access('view', 'taxonomy_term', $entity)) {
        // Theme entity label.
        return eckextend_theme_property(entity_label('taxonomy_term', $entity), $property, $vars['entity']);
      }
    }
  }
}
