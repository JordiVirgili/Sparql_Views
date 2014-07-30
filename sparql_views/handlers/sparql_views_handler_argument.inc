<?php
/**
 * SPARQL Views arguments.
 *
 * The argument enables re-writing the argument. For instance, if someone goes
 * to http://example.org/Pittsburgh, this argument could be rewritten to
 * http://dbpedia.org/resource/Pittsburgh.
 * 
 * Jordi Virgili (30/07/2014)
 * In Advanced, the Contextual Type "Number" has been modified to perform a Regex search instead of the use of the "=" operator.
 * The query when parameters are added to the URL now is "...WHERE { FILTER regex([attribute],  "[search]",  "i") }...".
 * This allows to perform partial searches.
 *
 * 
 * 
 */
class sparql_views_handler_argument extends views_handler_argument {
  // We will use the value type and language form elements for getting
  // necessary info about replacements that this argument makes.
  var $selected_sparql_options = array('value_type', 'language');

  /**
   * Set the options variables.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['sparql_options']['code'] = array('default' => '');
    $options = _sparql_views_attach_form_options($options, $this->selected_sparql_options);

    return $options;
  }

  /**
   * Provide the form for setting options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Attach the SPARQL specific form elements.
    $form = _sparql_views_attach_form_elements($form, $this->options, $this->selected_sparql_options);

    $form['sparql_options']['code'] = array(
      '#type' => 'textarea',
      '#title' => t('PHP argument rewrite code'),
      '#default_value' => $this->options['sparql_options']['code'],
      '#description' => t('Enter PHP code that returns a value to use for this filter. Do not use &lt;?php ?&gt;. You must return only a single value for just this filter. You can use $this->argument in your return. For example, return "http://dbpedia.org/resource/" . $this->argument.'),
    );

    $this->check_access($form, 'code');
  }

  /**
   * If we don't have access to the form but are showing it anyway, ensure that
   * the form is safe and cannot be changed from user input.
   */
  function check_access(&$form, $option_name) {
    if (!user_access('use PHP for settings')) {
      $form['sparql_options'][$option_name]['#disabled'] = TRUE;
      $form['sparql_options'][$option_name]['#value'] = $form[$this->option_name]['#default_value'];
      $form['sparql_options'][$option_name]['#description'] .= ' <strong>' . t('Note: you do not have permission to modify this. If you change the default argument type, this setting will be lost and you will NOT be able to get it back.') . '</strong>';
    }
  }

  /**
   * Set up the query for this argument.
   *
   * The argument sent may be found at $this->argument.
   */
  function query() {
    ob_start();
    $return = eval($this->options['sparql_options']['code']);
    ob_end_clean();

    $replacement = !empty($return) ? $return : $this->argument;
    $relationship = $this->options['relationship'];

    if ($relationship == 'none') {
      $subject = $this->definition['subject'];
    }
    else {
      $subject = $relationship;
    }
    $predicate = $this->definition['rdf_mapping'][0];

    if (!empty($replacement)) {
      $value_type = $this->options['sparql_options']['value_type'];
      $language = $this->options['sparql_options']['language'];

      if ($value_type != 'number') {
        $object = $this->query->format_replacement($replacement, $value_type, $language);
      }
	  // the numeric type has been modified to allow regex filtering. I know that is not the cleanest solution.... but... who cares about?
      else {
        $var = '?var' . rand();
		$varf = 'regex(' . $var;
        $this->query->add_regex(',', array($varf, $replacement));
        $object = $var;
      }

      $this->query->add_triple($subject, $predicate, $object);
    }
  }
}