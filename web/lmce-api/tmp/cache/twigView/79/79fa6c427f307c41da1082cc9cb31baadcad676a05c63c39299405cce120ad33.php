<?php

/* /var/www/lmce-api/vendor/cakephp/bake/src/Template/Bake/Layout/default.twig */
class __TwigTemplate_db4e068e344ebb3cf74f7f8eabf161699ad96de824248359a64f8d619a3b6482 extends Twig_Template
{
    public function __construct(Twig_Environment $env)
    {
        parent::__construct($env);

        $this->parent = false;

        $this->blocks = array(
        );
    }

    protected function doDisplay(array $context, array $blocks = array())
    {
        $__internal_28e9ce5ecc2f3c5130bb0c2b17ab725789e29560f12a03bedee84e48b892d48d = $this->env->getExtension("WyriHaximus\\TwigView\\Lib\\Twig\\Extension\\Profiler");
        $__internal_28e9ce5ecc2f3c5130bb0c2b17ab725789e29560f12a03bedee84e48b892d48d->enter($__internal_28e9ce5ecc2f3c5130bb0c2b17ab725789e29560f12a03bedee84e48b892d48d_prof = new Twig_Profiler_Profile($this->getTemplateName(), "template", "/var/www/lmce-api/vendor/cakephp/bake/src/Template/Bake/Layout/default.twig"));

        // line 16
        echo $this->getAttribute(($context["_view"] ?? null), "fetch", array(0 => "content"), "method");
        
        $__internal_28e9ce5ecc2f3c5130bb0c2b17ab725789e29560f12a03bedee84e48b892d48d->leave($__internal_28e9ce5ecc2f3c5130bb0c2b17ab725789e29560f12a03bedee84e48b892d48d_prof);

    }

    public function getTemplateName()
    {
        return "/var/www/lmce-api/vendor/cakephp/bake/src/Template/Bake/Layout/default.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  22 => 16,);
    }

    /** @deprecated since 1.27 (to be removed in 2.0). Use getSourceContext() instead */
    public function getSource()
    {
        @trigger_error('The '.__METHOD__.' method is deprecated since version 1.27 and will be removed in 2.0. Use getSourceContext() instead.', E_USER_DEPRECATED);

        return $this->getSourceContext()->getCode();
    }

    public function getSourceContext()
    {
        return new Twig_Source("{#
/**
 * CakePHP(tm) : Rapid Development Framework (http://cakephp.org)
 * Copyright (c) Cake Software Foundation, Inc. (http://cakefoundation.org)
 *
 * Licensed under The MIT License
 * For full copyright and license information, please see the LICENSE.txt
 * Redistributions of files must retain the above copyright notice.
 *
 * @copyright     Copyright (c) Cake Software Foundation, Inc. (http://cakefoundation.org)
 * @link          http://cakephp.org CakePHP(tm) Project
 * @since         2.0.0
 * @license       http://www.opensource.org/licenses/mit-license.php MIT License
 */
#}
{{ _view.fetch('content')|raw }}", "/var/www/lmce-api/vendor/cakephp/bake/src/Template/Bake/Layout/default.twig", "");
    }
}
