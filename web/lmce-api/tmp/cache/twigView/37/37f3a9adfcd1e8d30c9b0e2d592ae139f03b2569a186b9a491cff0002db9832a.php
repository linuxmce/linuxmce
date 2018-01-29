<?php

/* /var/www/lmce-api/vendor/cakephp/bake/src/Template/Bake/Template/edit.twig */
class __TwigTemplate_88f8b0e2a765acfd3ccb46ba51654aaa5962b51da99fadf52f9abf5d1399e4b4 extends Twig_Template
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
        $__internal_a62c8ac17cfc026459af6b8bffa370e29d0ee5e8cc883a69a0b918f12f425bbf = $this->env->getExtension("WyriHaximus\\TwigView\\Lib\\Twig\\Extension\\Profiler");
        $__internal_a62c8ac17cfc026459af6b8bffa370e29d0ee5e8cc883a69a0b918f12f425bbf->enter($__internal_a62c8ac17cfc026459af6b8bffa370e29d0ee5e8cc883a69a0b918f12f425bbf_prof = new Twig_Profiler_Profile($this->getTemplateName(), "template", "/var/www/lmce-api/vendor/cakephp/bake/src/Template/Bake/Template/edit.twig"));

        // line 16
        echo "<?php
/**
 * @var \\";
        // line 18
        echo twig_escape_filter($this->env, ($context["namespace"] ?? null), "html", null, true);
        echo "\\View\\AppView \$this
 * @var \\";
        // line 19
        echo twig_escape_filter($this->env, ($context["entityClass"] ?? null), "html", null, true);
        echo " \$";
        echo twig_escape_filter($this->env, ($context["singularVar"] ?? null), "html", null, true);
        echo "
 */
?>
";
        // line 22
echo $context['_view']->element("form",array(),array());
        
        $__internal_a62c8ac17cfc026459af6b8bffa370e29d0ee5e8cc883a69a0b918f12f425bbf->leave($__internal_a62c8ac17cfc026459af6b8bffa370e29d0ee5e8cc883a69a0b918f12f425bbf_prof);

    }

    public function getTemplateName()
    {
        return "/var/www/lmce-api/vendor/cakephp/bake/src/Template/Bake/Template/edit.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  38 => 22,  30 => 19,  26 => 18,  22 => 16,);
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
<?php
/**
 * @var \\{{ namespace }}\\View\\AppView \$this
 * @var \\{{ entityClass }} \${{ singularVar }}
 */
?>
{% element 'form' %}
", "/var/www/lmce-api/vendor/cakephp/bake/src/Template/Bake/Template/edit.twig", "");
    }
}
