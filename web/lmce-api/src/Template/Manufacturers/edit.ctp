<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface $manufacturer
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Form->postLink(
                __('Delete'),
                ['action' => 'delete', $manufacturer->PK_Manufacturer],
                ['confirm' => __('Are you sure you want to delete # {0}?', $manufacturer->PK_Manufacturer)]
            )
        ?></li>
        <li><?= $this->Html->link(__('List Manufacturers'), ['action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('List Device Template'), ['controller' => 'Devicetemplates', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device Template'), ['controller' => 'Devicetemplates', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Devices'), ['controller' => 'Devices', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device'), ['controller' => 'Devices', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="manufacturers form large-9 medium-8 columns content">
    <?= $this->Form->create($manufacturer) ?>
    <fieldset>
        <legend><?= __('Edit Manufacturer') ?></legend>
        <?php
            echo $this->Form->control('Description');
            echo $this->Form->control('IRFrequency');
            echo $this->Form->control('psc_id');
            echo $this->Form->control('psc_batch');
            echo $this->Form->control('psc_user');
            echo $this->Form->control('psc_frozen');
            echo $this->Form->control('psc_mod');
            echo $this->Form->control('psc_restrict');
            echo $this->Form->control('devices._ids', ['options' => $devices]);
        ?>
    </fieldset>
    <?= $this->Form->button(__('Submit')) ?>
    <?= $this->Form->end() ?>
</div>
