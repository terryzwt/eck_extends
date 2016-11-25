<?php

/**
 * @file
 * Implementation of 'Status' property behavior plugin for eck.
 */

$plugin = array(
  'label' => t('Boolean (field replica)'),
  'default_widget' => 'eckextend_boolean_property_widget',
  'default_formatter' => 'eckextend_property_default_formatter',
  'property_form' => 'eckextend_boolean_property_form',
);

/**
 * Implements 'default_widget' plugin.
 */
function eckextend_boolean_property_widget($property, $vars) {
  if ($settings = eckextend_property_settings($vars['entity']->entityType(), $property)) {
    $element = array(
      '#type' => $settings['type'],
      '#required' => $settings['required'],
      '#title' => t('@label', array('@label' => $vars['properties'][$property]['label'])),
      '#default_value' => eckextend_get_property_value($vars['entity'], $property),
      '#access' => eckextend_property_access($vars['entity'], $property, 'edit'),
      '#element_validate' => array('eckextend_null_property'),
    );

    switch ($settings['type']) {

      // Single on/off checkbox.
      case 'checkbox':
        if ($settings['display_label']) {
          $element['#title'] = $settings['on'];
        }
        break;

      // Radios.
      case 'radios':
        $options = array(
          '1' => $settings['on'],
          '0' => $settings['off'],
        );
        if (!$settings['required']) {
          $options = array('' => t('N/A')) + $options;
        }
        $element['#options'] = $options;
        break;
    }

    return $element;
  }
}

/**
 * Implements 'property_form' plugin.
 */
function eckextend_boolean_property_form($property, $vars) {
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
      'radios' => t('Radio buttons'),
      'checkbox' => t('Single on/off checkbox'),
    ),
    '#default_value' => isset($settings['type']) ? $settings['type'] : NULL,
  );
  $form['display_label'] = array(
    '#title' => t('Use property label instead of the "On value" as label'),
    '#type' => 'checkbox',
    '#default_value' => isset($settings['display_label']) ? $settings['display_label'] : NULL,
    '#states' => array(
      'visible' => array(
        ':input[name="property[settings][type]"]' => array('value' => 'checkbox'),
      ),
    ),
  );
  $form['on'] = array(
    '#title' => t('On value'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#maxlenght' => 128,
    '#default_value' => isset($settings['on']) ? $settings['on'] : '1',
  );
  $form['off'] = array(
    '#title' => t('Off value'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#maxlenght' => 128,
    '#default_value' => isset($settings['off']) ? $settings['off'] : '0',
  );

  return $form;
}
