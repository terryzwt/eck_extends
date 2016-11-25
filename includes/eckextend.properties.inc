<?php

/**
 * @file
 * Eckextend ECK Entity properties operations.
 */

/**
 * Form builder "eckextend_properties_form". List all properties.
 *
 * @see eckextend_properties_form_submit()
 */
function eckextend_properties($type_name = NULL) {
  // @TODO: Add a link to edit link for managing default properties.

  // Validate entity type.
  $entity_type = EckEntityType::loadByName($type_name);
  if (!$entity_type) {
    drupal_not_found();
    die();
  }

  $content = array(
    '#theme' => 'table',
    '#header' => array(
      'Label',
      'Machine name',
      'Behavior',
      'Type',
      'Actions'),
    '#rows' => array(),
  );

  // Prepare some vars to reuse them in action links.
  $opt = array('query' => drupal_get_destination());
  $path = "admin/structure/entity-type/{$type_name}/properties";
  // Loop trough properties and fill the rows.
  foreach ($entity_type->properties as $name => $info) {
    $actions = array(
      l(t('edit'), "$path/edit/{$name}", $opt),
      l(t('delete'), "$path/delete/{$name}", $opt),
    );
    $content['#rows'][] = array(
      $info['label'],
      $name,
      $info['behavior'],
      $info['type'],
      implode(' | ', $actions),
    );
  }

  return drupal_render($content);
}

/**
 * EDIT: Form builder "eckextend_property_form". Edit property.
 *
 * @see eckextend_property_form_submit()
 */
function eckextend_property_edit_form($form, $form_state, $type_name, $property) {
  // Validate entity type and property name.
  $entity_type = EckEntityType::loadByName($type_name);
  if (!$entity_type || !isset($entity_type->properties[$property])) {
    drupal_not_found();
    die;
  }

  // Pass the EckEntityType as a value.
  $form['entity_type'] = array('#type' => 'value', '#value' => $entity_type);
  $form['name'] = array('#type' => 'value', '#value' => $property);

  $form['property'] = _eckextend_get_property_form($entity_type, $property, $form_state);
  $form['property']['#tree'] = TRUE;
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * EDIT: Submit handler for "eckextend_properties_form" form.
 *
 * @see eckextend_property_form()
 */
function eckextend_property_edit_form_submit($form, $form_state) {
  $name = $form_state['values']['name'];
  $data = $form_state['values']['property'];
  $entity_type = $form_state['values']['entity_type'];
  // Update property and save.
  $entity_type->addPropertyData($name, $data);
  $entity_type->save();
  drupal_set_message(t('Property %name saved.', array('%name' => $name)));
}

/**
 * Helper: generate form elements for a property.
 */
function _eckextend_get_property_form($entity_type, $name, $form_state) {
  $form = array();
  if (!isset($entity_type->properties[$name])) {
    return FALSE;
  }
  $info = $entity_type->properties[$name];
  $form['info'] = array('#markup' => "{$info['label']} ({$name})");
  $form['behavior'] = array('#type' => 'value', '#value' => $info['behavior']);
  $form['type'] = array('#type' => 'value', '#value' => $info['type']);
  // Allow property label change.
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#required' => TRUE,
    '#max_lenght' => 30,
    '#default_value' => $info['label'],
  );

  $vars = array(
    'form_state' => $form_state,
    'property' => $name,
    'settings' => isset($info['settings']) ? $info['settings'] : NULL,
    'entity_type' => $entity_type,
    'behavior' => $info['behavior'],
  );

  if ($property_form = eckextend_property_behavior_invoke_plugin('property_form', $vars)) {
    $form['settings'] = $property_form;
    $form['settings']['#tree'] = TRUE;
  }

  return $form;
}

/**
 * ADD: Form builder "eckextend_property_add_form".
 *
 * @see eckextend_property_add_form_submit()
 */
function eckextend_property_add_form($form, $form_state, $type_name) {
  // Validate entity type.
  $entity_type = EckEntityType::loadByName($type_name);
  if (!$entity_type) {
    drupal_not_found();
    die;
  }

  // Ease access to $form_state['values'].
  $values = isset($form_state['values']) ? $form_state['values'] : array();

  // Pass the ECKEntityType object as value.
  $form['entity_type'] = array('#type' => 'value', '#value' => $entity_type);

  // Container for all entity property info.
  $form['property']['#tree'] = TRUE;
  $form['property']['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#required' => TRUE,
  );
  $form['property']['name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Name'),
    '#machine_name' => array(
      'source' => array('property','label'),
    ),
    '#element_validate' => array('_eckextend_property_exists'),
    '#required' => TRUE,
  );

  $behaviors = array();
  $property_behaviors = ctools_get_plugins('eck', 'property_behavior');
  foreach ($property_behaviors as $property_behavior) {
    $behaviors[$property_behavior['name']] = $property_behavior['label'];
  }
  $form['property']['behavior'] = array(
    '#type' => 'select',
    '#title' => t('Behavior'),
    '#options' => $behaviors,
    '#required' => TRUE,
    '#ajax' => array(
      'wrapper' => 'property-settings',
      'callback' => 'eckextend_property_callback',
    ),
  );
  $form['property']['settings']['#prefix'] = '<div id="property-settings">';
  $form['property']['settings']['#suffix'] = '</div>';

  // Provide the property form as implemented in the behavior.
  if (isset($values['property']['behavior'])) {
    // Prepare the context.
    $vars = array(
      'entity_type' => $entity_type,
      'behavior' => $values['property']['behavior'],
      'form_state' => $form_state,
    );

    // Invoke the "property_form" plugin function.
    if ($property_form = eckextend_property_behavior_invoke_plugin('property_form', $vars)) {
      $form['property']['settings'] = $property_form;
      $form['property']['settings']['#type'] = 'fieldset';
      $form['property']['settings']['#title'] = t(':name behavior settings', array(':name' => $values['property']['behavior']));
    }
  }
  // Always add the property settings as an element.
  $form['property']['settings']['#tree'] = TRUE;
  $form['property']['settings']['#prefix'] = '<div id="property-settings">';
  $form['property']['settings']['#suffix'] = '</div>';

  $form['property']['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => eck_property_types(),
    '#required' => TRUE,
  );

  // Actions.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * ADD form, ajax callback for property settings form.
 */
function eckextend_property_callback($form, &$form_state) {
  return $form['property']['settings'];
}

/**
 * ADD: Submit handler for "eckextend_property_add_form" form.
 *
 * @see eckextend_property_add_form()
 */
function eckextend_property_add_form_submit($form, &$form_state) {
  $entity_type = $form_state['values']['entity_type'];
  $name = $form_state['values']['property']['name'];

  $data = $form_state['values']['property'];
  unset($data['name']);

  $entity_type->addPropertyData($name, $data);
  $entity_type->save();

  drupal_set_message(t('Property %name saved.', array('%name' => $name)));
  $form_state['redirect'] = "admin/structure/entity-type/{$entity_type->name}/properties";
}

/**
 * DELETE: Form builder "eckextend_property_delete_form".
 *
 * @see eckextend_property_delete_form_submit()
 */
function eckextend_property_delete_form($form, $form_state, $type_name, $property) {
  // Validate entity type and property name.
  $entity_type = EckEntityType::loadByName($type_name);
  if (!$entity_type || !isset($entity_type->properties[$property])) {
    drupal_not_found();
    die;
  }
  $form = array();
  // Pass the EckEntityType as a value.
  $form['entity_type'] = array('#type' => 'value', '#value' => $entity_type);
  $form['property'] = array('#type' => 'value', '#value' => $property);
  $message = t('Are you sure you want to delete %name property?', array('%name' => $property));
  $caption = $message . t("This action cannot be undone.");

  return confirm_form($form, $message, "admin/confing/entity-type/{$entity_type->name}/property", $caption, t('Delete'));
}

/**
 * DELETE: Submit handler for "eckextend_property_delete_form" form.
 *
 * @see eckextend_property_delete_form()
 */
function eckextend_property_delete_form_submit($form, &$form_state) {
  $entity_type = $form_state['values']['entity_type'];
  $name = $form_state['values']['property'];

  $entity_type->removeProperty($name);
  $entity_type->save();
}

/**
 * Element validate for property name. Must be unique per entity type.
 */
function _eckextend_property_exists($element, $form_state) {
  if (empty($element['#value'])) {
    return;
  }
  $value = $element['#value'];
  $entity_type = $form_state['values']['entity_type'];

  if (isset($entity_type->properties[$value])) {
    $arg = array('%name' => $value);
    form_error($element, t('Property %name already exists. Please choose another machine name for it.', $arg));
  }
}
