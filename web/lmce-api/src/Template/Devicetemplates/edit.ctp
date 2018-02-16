<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface $devicetemplate
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Form->postLink(
                __('Delete'),
                ['action' => 'delete', $devicetemplate->PK_DeviceTemplate],
                ['confirm' => __('Are you sure you want to delete # {0}?', $devicetemplate->PK_DeviceTemplate)]
            )
        ?></li>
        <li><?= $this->Html->link(__('List Devicetemplates'), ['action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('List Devicecategory'), ['controller' => 'Devicecategories', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Devicecategory'), ['controller' => 'Devicecategories', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Manufacturer'), ['controller' => 'Manufacturers', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Manufacturer'), ['controller' => 'Manufacturers', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Device Data'), ['controller' => 'Devicedata', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device Data'), ['controller' => 'Devicedata', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="devicetemplates form large-9 medium-8 columns content">
    <?= $this->Form->create($devicetemplate) ?>
    <fieldset>
        <legend><?= __('Edit Devicetemplate') ?></legend>
        <?php
            echo $this->Form->control('Description');
            echo $this->Form->control('Comments');
            echo $this->Form->control('FK_DeviceCategory', ['options' => $devicecategory]);
            echo $this->Form->control('FK_Manufacturer');
            echo $this->Form->control('Define');
            echo $this->Form->control('ImplementsDCE');
            echo $this->Form->control('IsEmbedded');
            echo $this->Form->control('CommandLine');
            echo $this->Form->control('IsPlugAndPlay');
            echo $this->Form->control('IsIPBased');
            echo $this->Form->control('IsPlugIn');
            echo $this->Form->control('IRFrequency');
            echo $this->Form->control('FK_StabilityStatus');
            echo $this->Form->control('FK_Package');
            echo $this->Form->control('DestinationDir');
            echo $this->Form->control('FK_Users_Maintainer');
            echo $this->Form->control('FK_OperatingSystem');
            echo $this->Form->control('FK_Distro');
            echo $this->Form->control('InheritsMacFromPC');
            echo $this->Form->control('ConfigureScript');
            echo $this->Form->control('ManufacturerURL');
            echo $this->Form->control('InternalURLSuffix');
            echo $this->Form->control('WizardURL');
            echo $this->Form->control('FK_Licensing');
            echo $this->Form->control('FK_InfraredGroup');
            echo $this->Form->control('FK_CommMethod');
            echo $this->Form->control('FK_PnpLevel');
            echo $this->Form->control('HelpURL');
            echo $this->Form->control('psc_id');
            echo $this->Form->control('psc_batch');
            echo $this->Form->control('psc_user');
            echo $this->Form->control('psc_frozen');
            echo $this->Form->control('psc_mod');
            echo $this->Form->control('psc_restrict');
            echo $this->Form->control('device_data._ids', ['options' => $deviceData]);
        ?>
    </fieldset>
    <?= $this->Form->button(__('Submit')) ?>
    <?= $this->Form->end() ?>
</div>
