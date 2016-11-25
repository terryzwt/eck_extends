<?php

/**
 * @file
 * Implementation of 'Date' property behavior plugin for eck.
 */

$plugin = array(
  'label' => 'Date (Unix timestamp) (field replica)',
  'default_widget' => 'eckextend_timestamp_property_widget',
  'default_formatter' => 'eckextend_timestamp_property_formatter',
  'property_info' => 'eckextend_timestamp_property_info',
  'property_form' => 'eckextend_timestamp_property_form',
);

/**
 * Implements 'property_info' plugin.
 */
function eckextend_timestamp_property_info($property, $vars) {
  $vars['properties'][$property]['type'] = 'date';
  return $vars;
}

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_timestamp_property_widget($property, $vars) {
  $entity_type = $vars['entity']->entityType();
  if ($settings = eckextend_property_settings($entity_type, $property)) {
    $class = "eckextend-timestamp-{$entity_type}-{$property}";
    $element = array(
      '#attributes' => array('class' => array($class)),
      '#type' => 'textfield',
      '#required' => $settings['required'],
      '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
      '#default_value' => eckextend_get_property_value($vars['entity'], $property),
      '#access' => eckextend_property_access($vars['entity'], $property, 'edit'),
      '#element_validate' => array(
        'eckextend_null_property',
        'eckextend_timestamp_property_element_validate',
      ),
    );

    // @TODO: Maybe submit the timestamp here via JS using altField?
    $element['timestamp'] = array(
      '#type' => 'hidden',
    );

    $js = FALSE;
    $js_data = array('type' => 'file', 'scope' => 'footer');
    $path = drupal_get_path('module', 'eckextend');
    if ($settings['type'] == 'datepicker') {
      $js = TRUE;
      drupal_add_library('system', 'ui.datepicker');
    }

    if ($settings['type'] == 'datetimepicker') {
      $js = TRUE;
      drupal_add_library('system', 'ui.datepicker');
      drupal_add_js($path . '/datetimepicker/jquery.datetimepicker.js', 'file');
      drupal_add_css($path . '/datetimepicker/jquery.datetimepicker.css', 'file');
    }

    if ($js) {
      $js_settings = array();
      $js_settings['eckextend'][$settings['type']][] = array(
        'class' => ".{$class}",
        'settings' => $settings[$settings['type']],
      );
      $element['#settings'] = $settings[$settings['type']];
      drupal_add_js($js_settings, 'setting');
      $path = drupal_get_path('module', 'eckextend');
      drupal_add_js($path . '/plugins/property_behavior/timestamp.js', $js_data);
    }

    return $element;
  }
}

/**
 * Implements 'default_formatter' plugin.
 */
function eckextend_timestamp_property_formatter($property, $vars) {
  $entity = $vars['entity'];
  $value = eckextend_get_property_value($entity, $property);
  return eckextend_theme_property($value, $property, $entity);
}

/**
 * Implements 'property_form' plugin.
 */
function eckextend_timestamp_property_form($property, $vars) {
  $settings = isset($vars['settings']) ? $vars['settings'] : NULL;
  $form = array();
  $form['required'] = array(
    '#title' => t('Required property'),
    '#type' => 'checkbox',
    '#default_value' => isset($settings['required']) ? $settings['required'] : NULL,
  );
  $form['type'] = array(
    '#title' => t('Widget type (element)'),
    '#type' => 'select',
    '#options' => array(
      'textfield' => t('Text field'),
      'datepicker' => t('jQuery Datepicker'),
      'datetimepicker' => t('jQuery Date and Time picker'),
    ),
    '#default_value' => isset($settings['type']) ? $settings['type'] : NULL,
  );

  // jQuery Datepicker (http://api.jqueryui.com/datepicker).
  $form['datepicker'] = array(
    '#type' => 'fieldset',
    '#title' => t('Datepicker Widget'),
    '#states' => array(
      'visible' => array(
        ':input[name="property[settings][type]"]' => array('value' => 'datepicker'),
      ),
    ),
  );
  $form['datepicker']['dateFormat'] = array(
    '#title' => t('dateFormat'),
    '#type' => 'select',
    '#options' => date_popup_formats(),
    '#default_value' => isset($settings['datepicker']['dateFormat']) ? $settings['datepicker']['dateFormat'] : NULL,
  );
  $datepicker = array(
    'showButtonPanel' => 'Whether to display a button pane underneath the calendar.',
    'changeMonth' => 'Whether the month should be rendered as a dropdown instead of text.',
    'changeYear' => 'Whether the year should be rendered as a dropdown instead of text.',
    'firstDay' => 'Set the first day of the week. Unchecked: Sunday, Checked Monday.',
  );
  foreach ($datepicker as $option => $desc) {
    $form['datepicker'][$option] = array(
      '#title' => $option,
      '#description' => $desc,
      '#type' => 'checkbox',
      '#default_value' => isset($settings['datepicker'][$option]) ? $settings['datepicker'][$option] : NULL,
    );
  }

  // jQuery Date and Time picker (http://plugins.jquery.com/datetimepicker/).
  $form['datetimepicker'] = array(
    '#type' => 'fieldset',
    '#title' => t('jQuery Date and Time picker'),
    '#states' => array(
      'visible' => array(
        ':input[name="property[settings][type]"]' => array('value' => 'datetimepicker'),
      ),
    ),
  );
  $form['datetimepicker']['format'] = array(
    '#title' => t('dateFormat'),
    '#type' => 'select',
    '#options' => date_popup_formats(),
    '#default_value' => isset($settings['datetimepicker']['format']) ? $settings['datetimepicker']['format'] : NULL,
  );
  $datetimepicker = array(
    'datepicker' => 'Show datepicker.',
    'timepicker' => 'Show timepicker.',
    'weeks' => 'Show week number.',
    'opened' => 'Show week number.',
    'todayButton' => 'Show button "Go To Today".',
    'defaultSelect' => 'Highlight the current date even if the input is empty. 1',
    'timepickerScrollbar' => 'Show Timepicker Scrollbar. 1',
    'dayOfWeekStart' => 'First day of the week. Unchecked: Sunday, Checked Monday.',
  );
  foreach ($datetimepicker as $option => $desc) {
    $form['datetimepicker'][$option] = array(
      '#title' => $option,
      '#description' => $desc,
      '#type' => 'checkbox',
      '#default_value' => isset($settings['datetimepicker'][$option]) ? $settings['datetimepicker'][$option] : NULL,
    );
  }

  return $form;
}

/**
 * Timestamp property element validate.
 */
function eckextend_timestamp_property_element_validate($element, &$form_state) {
  if (empty($element['#value'])) {
    form_set_value($element, NULL, $form_state);
  }

  // @TODO: Get the value from timestamp element?
  $elm = implode('][', $element['#parents']);
  form_set_error($elm, 'Behavior under development :)');
}
