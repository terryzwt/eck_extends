<?php

/**
 * @file
 * Implementation of 'Reference' property behavior plugin for eck.
 */

$plugin = array(
  'label' => 'Reference (field replica)',
  'default_widget' => 'eckextend_reference_property_widget',
  'default_formatter' => 'eckextend_reference_property_formatter',
  'property_form' => 'eckextend_reference_property_form',
);

/**
 * Provide plugin specific form elements.
 */
function eckextend_reference_property_form($property, $vars) {
  $settings = isset($vars['settings']) ? $vars['settings'] : NULL;

  // Handle default values.
  $reference_entity_type = NULL;
  // First check initial saved values.
  if ($property && isset($settings['target_type'])) {
    $reference_entity_type = $settings['target_type'];
  }
  // Handle form submission values.
  if (isset($vars['form_state']['values']['property']['settings']['target_type'])) {
    $reference_entity_type = $vars['form_state']['values']['property']['settings']['target_type'];
  }

  // Prepare a select of entity types.
  $entity_options = array();
  $entity_info = entity_get_info();
  foreach ($entity_info as $type => $info) {
    $entity_options[$type] = $info['label'];
  }

  // Property specific form.
  $form = array();
  $form['required'] = array(
    '#title' => t('Required property'),
    '#type' => 'checkbox',
    '#default_value' => isset($settings['required']) ? $settings['required'] : NULL,
  );
  $form['type'] = array(
    '#title' => t('Widget Type'),
    '#type' => 'select',
    '#description' => t('The type of form element you would like to present to the user when creating this property.'),
    '#options' => array(
      'select' => t('Select list'),
      'radios' => t('Radios'),
    ),
    '#default_value' => isset($settings['type']) ? $settings['type'] : NULL,
  );
  if (module_exists('entityreference_autocomplete')) {
    $form['type']['#options']['entityreference'] = t('Entityreference autocomplete');
  }

  // Add target_type and target_bundles like entityreference field does.
  // Entity target type.
  $form['target_type'] = array(
    '#type' => 'select',
    '#title' => t('Target entity type'),
    '#options' => $entity_options,
    '#description' => t('The entity type that can be referenced through this property.'),
    '#default_value' => $reference_entity_type,
    '#required' => TRUE,
    '#ajax' => array(
      'wrapper' => 'target-bundles',
      'callback' => '_eckextend_reference_property_callback',
    ),
  );
  // Entity type target bundles.
  if ($reference_entity_type && isset($entity_options[$reference_entity_type])) {
    $entity_bundles = array();
    foreach ($entity_info[$reference_entity_type]['bundles'] as $bundle => $info) {
      $entity_bundles[$bundle] = $info['label'];
    }
    $form['target_bundles'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Target entity bundles'),
      '#description' => T('The bundles of the entity type that can be referenced. Optional, leave empty for all bundles.'),
      '#options' => $entity_bundles,
      '#default_value' => isset($settings['target_bundles']) ? $settings['target_bundles'] : NULL,
    );
  }
  // Always add "target_bundles" element for AJAX functionality.
  $form['target_bundles']['#prefix'] = '<div id="target-bundles">';
  $form['target_bundles']['#suffix'] = '</div>';

  return $form;
}

/**
 * Ajax callback for target type.
 */
function _eckextend_reference_property_callback($form, $form_state) {
  return $form['property']['settings']['target_bundles'];
}

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_reference_property_widget($property, $vars) {
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
    $target_type = $settings['target_type'];
    $bundles = drupal_map_assoc(array_filter($settings['target_bundles']));
    switch ($settings['type']) {
      // Entityreference autocomplete specific.
      case 'entityreference':
        $element['#era_entity_type'] = $settings['target_type'];
        $element['#era_bundles'] = $settings['target_type'];
        $element['#element_validate'] = array('_eckextend_reference_entityreference_validate');
        break;

      // Add options checkboxes.
      case 'radios':
        $entity_list = _eckextend_reference_entity_list($target_type, $bundles);
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
        $element['#options'] = _eckextend_reference_entity_list($target_type, $bundles);
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
function eckextend_reference_property_formatter($property, $vars) {
  // Get property settings.
  if ($settings = eckextend_property_settings($vars['entity']->entityType(), $property)) {
    // Get property value, in this case an entity ID.
    if ($entity_id = eckextend_get_property_value($vars['entity'], $property)) {
      // Load the entity and check for entity view access.
      $entity = entity_load_single($settings['target_type'], $entity_id);
      if (entity_access('view', $settings['target_type'], $entity)) {
        // Theme entity label.
        return eckextend_theme_property(entity_label($settings['target_type'], $entity), $property, $vars['entity']);
      }
    }
  }
}

/**
 * Build an entity list keyed by entity id, valued by entity label.
 */
function _eckextend_reference_entity_list($entity_type, $bundles = array(), $limit = 50) {
  $entity_list = array();

  // Get entity metadata, to be used for some checks.
  $entity_info = entity_get_info($entity_type);

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', $entity_type);

  // Add the 'bundle' condition, validate entity key for bundles.
  if ($bundles && !empty($entity_info['entity keys']['bundle'])) {
    $query->entityCondition('bundle', $bundles);
  }

  // Set the maximum number of results returned.
  if ($limit && is_numeric($limit)) {
    $query->range(0, $limit);
  }

  // Execute query and log any unexpected errors.
  try {
    $result = $query->execute();
  }
  catch (Exception $e) {
    $args = array('@error' => $e->getMessage());
    watchdog(WATCHDOG_ERROR, 'Eckextend reference error: @error', $args);
  }

  if (!empty($result[$entity_type])) {
    $entities = entity_load($entity_type, array_keys($result[$entity_type]));
    // Iterate through all entities making sure user has access.
    foreach ($entities as $entity_id => $entity) {
      if (entity_access('view', $entity_type, $entity)) {
        $entity_list[$entity_id] = entity_label($entity_type, $entity);
      }
    }
  }

  return $entity_list;
}

/**
 * Custom form element validator for 'reference' behavior via 'entityreference'.
 */
function _eckextend_reference_entityreference_validate($element, &$form_state) {
  // Make sure we have values.
  if (empty($element['#value'])) {
    form_set_value($element, NULL, $form_state);
    return;
  }
  // Get the entity ID at the end of our string.
  if (preg_match("/.+\((\d+)\)/", $element['#value'], $matches)) {
    // Set the value in form_state.
    form_set_value($element, $matches[1], $form_state);
  }
  else {
    // Empty the selected input.
    form_set_value($element, NULL, $form_state);
    $error = t('Please select a valid option for @elm', array('@elm' => $element['#title']));
    form_set_error($element['#name'], $error);
  }
}
