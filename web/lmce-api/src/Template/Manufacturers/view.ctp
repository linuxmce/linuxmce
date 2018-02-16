<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface $manufacturer
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('Edit Manufacturer'), ['action' => 'edit', $manufacturer->PK_Manufacturer]) ?> </li>
        <li><?= $this->Form->postLink(__('Delete Manufacturer'), ['action' => 'delete', $manufacturer->PK_Manufacturer], ['confirm' => __('Are you sure you want to delete # {0}?', $manufacturer->PK_Manufacturer)]) ?> </li>
        <li><?= $this->Html->link(__('List Manufacturers'), ['action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Manufacturer'), ['action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Device Template'), ['controller' => 'Devicetemplates', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Device Template'), ['controller' => 'Devicetemplates', 'action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Devices'), ['controller' => 'Devices', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Device'), ['controller' => 'Devices', 'action' => 'add']) ?> </li>
    </ul>
</nav>
<div class="manufacturers view large-9 medium-8 columns content">
    <h3><?= h($manufacturer->PK_Manufacturer) ?></h3>
    <table class="vertical-table">
        <tr>
            <th scope="row"><?= __('Description') ?></th>
            <td><?= h($manufacturer->Description) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('PK Manufacturer') ?></th>
            <td><?= $this->Number->format($manufacturer->PK_Manufacturer) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('IRFrequency') ?></th>
            <td><?= $this->Number->format($manufacturer->IRFrequency) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Id') ?></th>
            <td><?= $this->Number->format($manufacturer->psc_id) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Batch') ?></th>
            <td><?= $this->Number->format($manufacturer->psc_batch) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc User') ?></th>
            <td><?= $this->Number->format($manufacturer->psc_user) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Restrict') ?></th>
            <td><?= $this->Number->format($manufacturer->psc_restrict) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Mod') ?></th>
            <td><?= h($manufacturer->psc_mod) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Frozen') ?></th>
            <td><?= $manufacturer->psc_frozen ? __('Yes') : __('No'); ?></td>
        </tr>
    </table>
    <div class="related">
        <h4><?= __('Related Devices') ?></h4>
        <?php if (!empty($manufacturer->devices)): ?>
        <table cellpadding="0" cellspacing="0">
            <tr>
                <th scope="col"><?= __('PK Device') ?></th>
                <th scope="col"><?= __('Disabled') ?></th>
                <th scope="col"><?= __('FK Room') ?></th>
                <th scope="col"><?= __('FK Installation') ?></th>
                <th scope="col"><?= __('FK DesignObj') ?></th>
                <th scope="col"><?= __('Description') ?></th>
                <th scope="col"><?= __('FK DeviceTemplate') ?></th>
                <th scope="col"><?= __('FK Device ControlledVia') ?></th>
                <th scope="col"><?= __('IPaddress') ?></th>
                <th scope="col"><?= __('MACaddress') ?></th>
                <th scope="col"><?= __('IgnoreOnOff') ?></th>
                <th scope="col"><?= __('FK Device RouteTo') ?></th>
                <th scope="col"><?= __('NeedConfigure') ?></th>
                <th scope="col"><?= __('State') ?></th>
                <th scope="col"><?= __('Status') ?></th>
                <th scope="col"><?= __('PingTest') ?></th>
                <th scope="col"><?= __('EK LicensedDevice') ?></th>
                <th scope="col"><?= __('IsNewDevice') ?></th>
                <th scope="col"><?= __('ManuallyConfigureEA') ?></th>
                <th scope="col"><?= __('Registered') ?></th>
                <th scope="col"><?= __('Psc Id') ?></th>
                <th scope="col"><?= __('Psc Batch') ?></th>
                <th scope="col"><?= __('Psc User') ?></th>
                <th scope="col"><?= __('Psc Frozen') ?></th>
                <th scope="col"><?= __('Psc Mod') ?></th>
                <th scope="col"><?= __('Psc Restrict') ?></th>
                <th scope="col" class="actions"><?= __('Actions') ?></th>
            </tr>
            <?php foreach ($manufacturer->devices as $devices): ?>
            <tr>
                <td><?= h($devices->PK_Device) ?></td>
                <td><?= h($devices->Disabled) ?></td>
                <td><?= h($devices->FK_Room) ?></td>
                <td><?= h($devices->FK_Installation) ?></td>
                <td><?= h($devices->FK_DesignObj) ?></td>
                <td><?= h($devices->Description) ?></td>
                <td><?= h($devices->FK_DeviceTemplate) ?></td>
                <td><?= h($devices->FK_Device_ControlledVia) ?></td>
                <td><?= h($devices->IPaddress) ?></td>
                <td><?= h($devices->MACaddress) ?></td>
                <td><?= h($devices->IgnoreOnOff) ?></td>
                <td><?= h($devices->FK_Device_RouteTo) ?></td>
                <td><?= h($devices->NeedConfigure) ?></td>
                <td><?= h($devices->State) ?></td>
                <td><?= h($devices->Status) ?></td>
                <td><?= h($devices->PingTest) ?></td>
                <td><?= h($devices->EK_LicensedDevice) ?></td>
                <td><?= h($devices->IsNewDevice) ?></td>
                <td><?= h($devices->ManuallyConfigureEA) ?></td>
                <td><?= h($devices->Registered) ?></td>
                <td><?= h($devices->psc_id) ?></td>
                <td><?= h($devices->psc_batch) ?></td>
                <td><?= h($devices->psc_user) ?></td>
                <td><?= h($devices->psc_frozen) ?></td>
                <td><?= h($devices->psc_mod) ?></td>
                <td><?= h($devices->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['controller' => 'Devices', 'action' => 'view', $devices->PK_Device]) ?>
                    <?= $this->Html->link(__('Edit'), ['controller' => 'Devices', 'action' => 'edit', $devices->PK_Device]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['controller' => 'Devices', 'action' => 'delete', $devices->PK_Device], ['confirm' => __('Are you sure you want to delete # {0}?', $devices->PK_Device)]) ?>
                </td>
            </tr>
            <?php endforeach; ?>
        </table>
        <?php endif; ?>
    </div>
    <div class="related">
        <h4><?= __('Related Devicetemplates') ?></h4>
        <?php if (!empty($manufacturer->device_template)): ?>
        <table cellpadding="0" cellspacing="0">
            <tr>
                <th scope="col"><?= __('PK DeviceTemplate') ?></th>
                <th scope="col"><?= __('Description') ?></th>
                <th scope="col"><?= __('Comments') ?></th>
                <th scope="col"><?= __('FK DeviceCategory') ?></th>
                <th scope="col"><?= __('FK Manufacturer') ?></th>
                <th scope="col"><?= __('Define') ?></th>
                <th scope="col"><?= __('ImplementsDCE') ?></th>
                <th scope="col"><?= __('IsEmbedded') ?></th>
                <th scope="col"><?= __('CommandLine') ?></th>
                <th scope="col"><?= __('IsPlugAndPlay') ?></th>
                <th scope="col"><?= __('IsIPBased') ?></th>
                <th scope="col"><?= __('IsPlugIn') ?></th>
                <th scope="col"><?= __('IRFrequency') ?></th>
                <th scope="col"><?= __('FK StabilityStatus') ?></th>
                <th scope="col"><?= __('FK Package') ?></th>
                <th scope="col"><?= __('DestinationDir') ?></th>
                <th scope="col"><?= __('FK Users Maintainer') ?></th>
                <th scope="col"><?= __('FK OperatingSystem') ?></th>
                <th scope="col"><?= __('FK Distro') ?></th>
                <th scope="col"><?= __('InheritsMacFromPC') ?></th>
                <th scope="col"><?= __('ConfigureScript') ?></th>
                <th scope="col"><?= __('ManufacturerURL') ?></th>
                <th scope="col"><?= __('InternalURLSuffix') ?></th>
                <th scope="col"><?= __('WizardURL') ?></th>
                <th scope="col"><?= __('FK Licensing') ?></th>
                <th scope="col"><?= __('FK InfraredGroup') ?></th>
                <th scope="col"><?= __('FK CommMethod') ?></th>
                <th scope="col"><?= __('FK PnpLevel') ?></th>
                <th scope="col"><?= __('HelpURL') ?></th>
                <th scope="col"><?= __('Psc Id') ?></th>
                <th scope="col"><?= __('Psc Batch') ?></th>
                <th scope="col"><?= __('Psc User') ?></th>
                <th scope="col"><?= __('Psc Frozen') ?></th>
                <th scope="col"><?= __('Psc Mod') ?></th>
                <th scope="col"><?= __('Psc Restrict') ?></th>
                <th scope="col" class="actions"><?= __('Actions') ?></th>
            </tr>
            <?php foreach ($manufacturer->device_template as $deviceTemplate): ?>
            <tr>
                <td><?= h($deviceTemplate->PK_DeviceTemplate) ?></td>
                <td><?= h($deviceTemplate->Description) ?></td>
                <td><?= h($deviceTemplate->Comments) ?></td>
                <td><?= h($deviceTemplate->FK_DeviceCategory) ?></td>
                <td><?= h($deviceTemplate->FK_Manufacturer) ?></td>
                <td><?= h($deviceTemplate->Define) ?></td>
                <td><?= h($deviceTemplate->ImplementsDCE) ?></td>
                <td><?= h($deviceTemplate->IsEmbedded) ?></td>
                <td><?= h($deviceTemplate->CommandLine) ?></td>
                <td><?= h($deviceTemplate->IsPlugAndPlay) ?></td>
                <td><?= h($deviceTemplate->IsIPBased) ?></td>
                <td><?= h($deviceTemplate->IsPlugIn) ?></td>
                <td><?= h($deviceTemplate->IRFrequency) ?></td>
                <td><?= h($deviceTemplate->FK_StabilityStatus) ?></td>
                <td><?= h($deviceTemplate->FK_Package) ?></td>
                <td><?= h($deviceTemplate->DestinationDir) ?></td>
                <td><?= h($deviceTemplate->FK_Users_Maintainer) ?></td>
                <td><?= h($deviceTemplate->FK_OperatingSystem) ?></td>
                <td><?= h($deviceTemplate->FK_Distro) ?></td>
                <td><?= h($deviceTemplate->InheritsMacFromPC) ?></td>
                <td><?= h($deviceTemplate->ConfigureScript) ?></td>
                <td><?= h($deviceTemplate->ManufacturerURL) ?></td>
                <td><?= h($deviceTemplate->InternalURLSuffix) ?></td>
                <td><?= h($deviceTemplate->WizardURL) ?></td>
                <td><?= h($deviceTemplate->FK_Licensing) ?></td>
                <td><?= h($deviceTemplate->FK_InfraredGroup) ?></td>
                <td><?= h($deviceTemplate->FK_CommMethod) ?></td>
                <td><?= h($deviceTemplate->FK_PnpLevel) ?></td>
                <td><?= h($deviceTemplate->HelpURL) ?></td>
                <td><?= h($deviceTemplate->psc_id) ?></td>
                <td><?= h($deviceTemplate->psc_batch) ?></td>
                <td><?= h($deviceTemplate->psc_user) ?></td>
                <td><?= h($deviceTemplate->psc_frozen) ?></td>
                <td><?= h($deviceTemplate->psc_mod) ?></td>
                <td><?= h($deviceTemplate->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['controller' => 'Devicetemplates', 'action' => 'view', $deviceTemplate->PK_DeviceTemplate]) ?>
                    <?= $this->Html->link(__('Edit'), ['controller' => 'Devicetemplates', 'action' => 'edit', $deviceTemplate->PK_DeviceTemplate]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['controller' => 'Devicetemplates', 'action' => 'delete', $deviceTemplate->PK_DeviceTemplate], ['confirm' => __('Are you sure you want to delete # {0}?', $deviceTemplate->PK_DeviceTemplate)]) ?>
                </td>
            </tr>
            <?php endforeach; ?>
        </table>
        <?php endif; ?>
    </div>
</div>
