<?php

/**
 * @file
 * Custom classes for ECK entities.
 */

/**
 * Custom version of EntityType allowing entity properties manipulation.
 */
class EckEntityType extends EntityType {

  public $changes;
  // All ECK propertt types.
  public $property_types;
  // All ECK behaviors.
  public $behaviors = array();

  // Constructor.
  public function __construct() {
    parent::__construct();
    $this->properties = array();
    $this->changes = array();
    $this->property_types = eck_property_types();
    $behaviors = ctools_get_plugins('eck', 'property_behavior');
    $this->behaviors = drupal_map_assoc(array_keys($behaviors));
  }

  /**
   * Use a custom loader, that will skip cache.
   */
  public static function loadByName($name) {
    $entity_types = EckEntityType::loadAll();
    if (isset($entity_types[$name])) {
      return $entity_types[$name];
    }

    return NULL;
  }

  /**
   * Use a custom loader, that will skip cache.
   */
  public static function loadAll($reset = FALSE) {
    $entity_types = array();
    $results = db_select('eck_entity_type', 't')
        ->fields('t', array('name'))
        ->execute();
    foreach ($results as $result) {
      $name = $result->name;
      $entity_type = new EckEntityType();
      $entity_type->load('name', $name);
      $entity_types[$name] = $entity_type;
    }
    return $entity_types;
  }

  /**
   * Custom method, allowing to save additional info in property.
   */
  public function addPropertyData($name, $data = NULL) {
    if (!$this->isNew) {
      $this->recordFieldChanges('add', $name);
    }

    // Validate required property keys.
    foreach (array('label', 'type', 'behavior') as $key) {
      if (!isset($data[$key]) || empty($data[$key])) {
        return FALSE;
      }
    }
    // Validate the property type.
    if (!isset($this->property_types[$data['type']])) {
      return FALSE;
    }
    // Validate the property behavior.
    if (!isset($this->behaviors[$data['behavior']])) {
      return FALSE;
    }

    $p = $p2 = $this->properties;
    // @todo check that type is an actual type.
    $p[$name] = $data;

    $this->properties = $p;

    // If the property did not exist let the behaviors execute some code.
    if (!array_key_exists($name, $p2)) {
      $params = array(
        'name' => $name,
        'property' => $p[$name],
        'entity_type' => $this,
      );
      eck_property_behavior_invoke_plugin($this, 'property_add', $params);
    }
  }

  /**
   * Overide RecordFieldChanges.
   */
  private function recordFieldChanges($op, $name) {
    // If it is not new we need to keep track of stuff.
    if (!$this->isNew) {
      $p = $this->properties;
      $c = $this->changes;
      switch ($op) {
        case 'add':
          // If the property does not exist already add keep track.
          if (!array_key_exists($name, $p)) {
            $c[$op][] = $name;
          }
          break;

        case 'remove':
          // If there is an add in the changes take it out, otherwise add a
          // remove.
          if (array_key_exists('add', $c)) {
            $key = array_search($name, $c['add']);
            if ($key != FALSE) {
              unset($c['add'][$key]);
            }
          }
          else {
            $c[$op][] = $name;
          }
          break;
      }
      $this->changes = $c;
    }
  }

  /**
   * Save.
   */
  public function save() {
    if ($this->isNew) {
      module_load_include('inc', 'eck', 'eck.entity_type');
      $schema = eck__entity_type__schema($this);
      db_create_table("eck_{$this->name}", $schema);

      // Allow other modules to respond to the creation of entity types.
      module_invoke_all('eck_entity_type_insert', $this);

    }
    else {
      // Modify the already existing table in accordance with the
      // recorded changes.
      if (array_key_exists('add', $this->changes)) {
        foreach ($this->changes['add'] as $name) {
          // First lets get the record.
          $properties = $this->properties;
          $property = $properties[$name];

          // Now we check to see whether it is a default or a custom property
          // it is not custom so lets get the schema and add the field.
          $schema = eck_property_type_schema($property['type']);
          db_add_field("eck_{$this->name}", $name, $schema);
        }
      }
      if (array_key_exists('remove', $this->changes)) {
        foreach ($this->changes['remove'] as $name) {
          db_drop_field("eck_{$this->name}", $name);
        }
      }

      // Allow other modules to respond to the change of entity types.
      module_invoke_all('eck_entity_type_update', $this);
    }

    parent::save();
    global $_eck_entity_types_cache;

    $cache_enabled = isset($_eck_entity_types_cache) ? TRUE : FALSE;

    if ($cache_enabled) {
      $_eck_entity_types_cache->reset();
    }

    variable_set('eck_clear_caches', TRUE);
  }

  public function removeProperty($name) {
    $p = $this->properties;
    if (array_key_exists($name, $p)) {

      // Let the behaviors execute some code.
      eck_property_behavior_invoke_plugin($this, 'property_remove',
      array('name' => $name, 'property' => $p[$name], 'entity_type' => $this));

      unset($p[$name]);
      $this->properties = $p;
      if (!$this->isNew) {
        $this->recordFieldChanges('remove', $name);
      }
    }
  }

}
