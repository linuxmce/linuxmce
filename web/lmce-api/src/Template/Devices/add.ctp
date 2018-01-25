<?php
/**
 * @var \App\View\AppView $this
 * @var \App\Model\Entity\Device $device
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('List Devices'), ['action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('List Device Template'), ['controller' => 'Devicetemplates', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device Template'), ['controller' => 'Devicetemplates', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Room'), ['controller' => 'Rooms', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Room'), ['controller' => 'Rooms', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Device Data'), ['controller' => 'Devicedata', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device Data'), ['controller' => 'Devicedata', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="devices form large-9 medium-8 columns content">
    <?= $this->Form->create($device) ?>
    <fieldset>
        <legend><?= __('Add Device') ?></legend>
        <?php
            echo $this->Form->control('Disabled');
            echo $this->Form->control('FK_Room');
            echo $this->Form->control('FK_Installation');
            echo $this->Form->control('FK_DesignObj');
            echo $this->Form->control('Description');
            echo $this->Form->control('FK_DeviceTemplate', ['options' => $deviceTemplate, 'empty' => true]);
            echo $this->Form->control('FK_Device_ControlledVia');
            echo $this->Form->control('IPaddress');
            echo $this->Form->control('MACaddress');
            echo $this->Form->control('IgnoreOnOff');
            echo $this->Form->control('FK_Device_RouteTo');
            echo $this->Form->control('NeedConfigure');
            echo $this->Form->control('State');
            echo $this->Form->control('Status');
            echo $this->Form->control('PingTest');
            echo $this->Form->control('EK_LicensedDevice');
            echo $this->Form->control('IsNewDevice');
            echo $this->Form->control('ManuallyConfigureEA');
            echo $this->Form->control('Registered');
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
