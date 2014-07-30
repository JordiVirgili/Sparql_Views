[Lin Clark](https://github.com/linclark)'s Sparql_Views (https://www.drupal.org/project/sparql_views) Modified by Jordi Virgili

SPARQL VIEWS modification
============


   * Now with an option to enable the selection of datasets. This modifies the query addind the clause [...]from [graph] [...] when a dataset is specified in the "SPARQL Endpoints Registry" area.
   This option was not implemented before.

   * In Advanced options (when editing a view), the Contextual Filter Type "Number" has been modified to perform a Regex search instead of the use of the "=" operator.
   The query when parameters are added to the URL now is "...WHERE { FILTER regex([attribute],  "[search]",  "i") }...".
   This allows to perform partial searches. The format for this search will be URL/parameter, adding a / for each new parameter.
