<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface $devicetemplate
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('Edit Devicetemplate'), ['action' => 'edit', $devicetemplate->PK_DeviceTemplate]) ?> </li>
        <li><?= $this->Form->postLink(__('Delete Devicetemplate'), ['action' => 'delete', $devicetemplate->PK_DeviceTemplate], ['confirm' => __('Are you sure you want to delete # {0}?', $devicetemplate->PK_DeviceTemplate)]) ?> </li>
        <li><?= $this->Html->link(__('List Devicetemplates'), ['action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Devicetemplate'), ['action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Devicecategory'), ['controller' => 'Devicecategories', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Devicecategory'), ['controller' => 'Devicecategories', 'action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Manufacturer'), ['controller' => 'Manufacturers', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Manufacturer'), ['controller' => 'Manufacturers', 'action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Device Data'), ['controller' => 'Devicedata', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Device Data'), ['controller' => 'Devicedata', 'action' => 'add']) ?> </li>
    </ul>
</nav>
<div class="devicetemplates view large-9 medium-8 columns content">
    <h3><?= h($devicetemplate->PK_DeviceTemplate) ?></h3>
    <table class="vertical-table">
        <tr>
            <th scope="row"><?= __('Description') ?></th>
            <td><?= h($devicetemplate->Description) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Devicecategory') ?></th>
            <td><?= $devicetemplate->has('devicecategory') ? $this->Html->link($devicetemplate->devicecategory->PK_DeviceCategory, ['controller' => 'Devicecategories', 'action' => 'view', $devicetemplate->devicecategory->PK_DeviceCategory]) : '' ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Define') ?></th>
            <td><?= h($devicetemplate->Define) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('CommandLine') ?></th>
            <td><?= h($devicetemplate->CommandLine) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('DestinationDir') ?></th>
            <td><?= h($devicetemplate->DestinationDir) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('ConfigureScript') ?></th>
            <td><?= h($devicetemplate->ConfigureScript) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('ManufacturerURL') ?></th>
            <td><?= h($devicetemplate->ManufacturerURL) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('InternalURLSuffix') ?></th>
            <td><?= h($devicetemplate->InternalURLSuffix) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('WizardURL') ?></th>
            <td><?= h($devicetemplate->WizardURL) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('HelpURL') ?></th>
            <td><?= h($devicetemplate->HelpURL) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Manufacturer') ?></th>
            <td><?= $devicetemplate->has('manufacturer') ? $this->Html->link($devicetemplate->manufacturer->PK_Manufacturer, ['controller' => 'Manufacturers', 'action' => 'view', $devicetemplate->manufacturer->PK_Manufacturer]) : '' ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('PK DeviceTemplate') ?></th>
            <td><?= $this->Number->format($devicetemplate->PK_DeviceTemplate) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Manufacturer') ?></th>
            <td><?= $this->Number->format($devicetemplate->FK_Manufacturer) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('ImplementsDCE') ?></th>
            <td><?= $this->Number->format($devicetemplate->ImplementsDCE) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('IsPlugAndPlay') ?></th>
            <td><?= $this->Number->format($devicetemplate->IsPlugAndPlay) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('IsIPBased') ?></th>
            <td><?= $this->Number->format($devicetemplate->IsIPBased) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('IsPlugIn') ?></th>
            <td><?= $this->Number->format($devicetemplate->IsPlugIn) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('IRFrequency') ?></th>
            <td><?= $this->Number->format($devicetemplate->IRFrequency) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK StabilityStatus') ?></th>
            <td><?= $this->Number->format($devicetemplate->FK_StabilityStatus) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Package') ?></th>
            <td><?= $this->Number->format($devicetemplate->FK_Package) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Users Maintainer') ?></th>
            <td><?= $this->Number->format($devicetemplate->FK_Users_Maintainer) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK OperatingSystem') ?></th>
            <td><?= $this->Number->format($devicetemplate->FK_OperatingSystem) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Distro') ?></th>
            <td><?= $this->Number->format($devicetemplate->FK_Distro) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Licensing') ?></th>
            <td><?= $this->Number->format($devicetemplate->FK_Licensing) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK InfraredGroup') ?></th>
            <td><?= $this->Number->format($devicetemplate->FK_InfraredGroup) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK CommMethod') ?></th>
            <td><?= $this->Number->format($devicetemplate->FK_CommMethod) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK PnpLevel') ?></th>
            <td><?= $this->Number->format($devicetemplate->FK_PnpLevel) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Id') ?></th>
            <td><?= $this->Number->format($devicetemplate->psc_id) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Batch') ?></th>
            <td><?= $this->Number->format($devicetemplate->psc_batch) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc User') ?></th>
            <td><?= $this->Number->format($devicetemplate->psc_user) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Restrict') ?></th>
            <td><?= $this->Number->format($devicetemplate->psc_restrict) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Mod') ?></th>
            <td><?= h($devicetemplate->psc_mod) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('IsEmbedded') ?></th>
            <td><?= $devicetemplate->IsEmbedded ? __('Yes') : __('No'); ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('InheritsMacFromPC') ?></th>
            <td><?= $devicetemplate->InheritsMacFromPC ? __('Yes') : __('No'); ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Frozen') ?></th>
            <td><?= $devicetemplate->psc_frozen ? __('Yes') : __('No'); ?></td>
        </tr>
    </table>
    <div class="row">
        <h4><?= __('Comments') ?></h4>
        <?= $this->Text->autoParagraph(h($devicetemplate->Comments)); ?>
    </div>
    <div class="related">
        <h4><?= __('Related Devicedata') ?></h4>
        <?php if (!empty($devicetemplate->device_data)): ?>
        <table cellpadding="0" cellspacing="0">
            <tr>
                <th scope="col"><?= __('PK DeviceData') ?></th>
                <th scope="col"><?= __('Description') ?></th>
                <th scope="col"><?= __('Define') ?></th>
                <th scope="col"><?= __('FK ParameterType') ?></th>
                <th scope="col"><?= __('Psc Id') ?></th>
                <th scope="col"><?= __('Psc Batch') ?></th>
                <th scope="col"><?= __('Psc User') ?></th>
                <th scope="col"><?= __('Psc Frozen') ?></th>
                <th scope="col"><?= __('Psc Mod') ?></th>
                <th scope="col"><?= __('Psc Restrict') ?></th>
                <th scope="col" class="actions"><?= __('Actions') ?></th>
            </tr>
            <?php foreach ($devicetemplate->device_data as $deviceData): ?>
            <tr>
                <td><?= h($deviceData->PK_DeviceData) ?></td>
                <td><?= h($deviceData->Description) ?></td>
                <td><?= h($deviceData->Define) ?></td>
                <td><?= h($deviceData->FK_ParameterType) ?></td>
                <td><?= h($deviceData->psc_id) ?></td>
                <td><?= h($deviceData->psc_batch) ?></td>
                <td><?= h($deviceData->psc_user) ?></td>
                <td><?= h($deviceData->psc_frozen) ?></td>
                <td><?= h($deviceData->psc_mod) ?></td>
                <td><?= h($deviceData->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['controller' => 'Devicedata', 'action' => 'view', $deviceData->PK_DeviceData]) ?>
                    <?= $this->Html->link(__('Edit'), ['controller' => 'Devicedata', 'action' => 'edit', $deviceData->PK_DeviceData]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['controller' => 'Devicedata', 'action' => 'delete', $deviceData->PK_DeviceData], ['confirm' => __('Are you sure you want to delete # {0}?', $deviceData->PK_DeviceData)]) ?>
                </td>
            </tr>
            <?php endforeach; ?>
        </table>
        <?php endif; ?>
    </div>
</div>
