<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface[]|\Cake\Collection\CollectionInterface $devicetemplates
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('New Devicetemplate'), ['action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Devicecategory'), ['controller' => 'Devicecategories', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Devicecategory'), ['controller' => 'Devicecategories', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Manufacturer'), ['controller' => 'Manufacturers', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Manufacturer'), ['controller' => 'Manufacturers', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Device Data'), ['controller' => 'Devicedata', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device Data'), ['controller' => 'Devicedata', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="devicetemplates index large-9 medium-8 columns content">
    <h3><?= __('Devicetemplates') ?></h3>
    <table cellpadding="0" cellspacing="0">
        <thead>
            <tr>
                <th scope="col"><?= $this->Paginator->sort('PK_DeviceTemplate') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Description') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_DeviceCategory') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Manufacturer') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Define') ?></th>
                <th scope="col"><?= $this->Paginator->sort('ImplementsDCE') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IsEmbedded') ?></th>
                <th scope="col"><?= $this->Paginator->sort('CommandLine') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IsPlugAndPlay') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IsIPBased') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IsPlugIn') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IRFrequency') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_StabilityStatus') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Package') ?></th>
                <th scope="col"><?= $this->Paginator->sort('DestinationDir') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Users_Maintainer') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_OperatingSystem') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Distro') ?></th>
                <th scope="col"><?= $this->Paginator->sort('InheritsMacFromPC') ?></th>
                <th scope="col"><?= $this->Paginator->sort('ConfigureScript') ?></th>
                <th scope="col"><?= $this->Paginator->sort('ManufacturerURL') ?></th>
                <th scope="col"><?= $this->Paginator->sort('InternalURLSuffix') ?></th>
                <th scope="col"><?= $this->Paginator->sort('WizardURL') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Licensing') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_InfraredGroup') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_CommMethod') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_PnpLevel') ?></th>
                <th scope="col"><?= $this->Paginator->sort('HelpURL') ?></th>
                <th scope="col"><?= $this->Paginator->sort('psc_id') ?></th>
                <th scope="col"><?= $this->Paginator->sort('psc_batch') ?></th>
                <th scope="col"><?= $this->Paginator->sort('psc_user') ?></th>
                <th scope="col"><?= $this->Paginator->sort('psc_frozen') ?></th>
                <th scope="col"><?= $this->Paginator->sort('psc_mod') ?></th>
                <th scope="col"><?= $this->Paginator->sort('psc_restrict') ?></th>
                <th scope="col" class="actions"><?= __('Actions') ?></th>
            </tr>
        </thead>
        <tbody>
            <?php foreach ($devicetemplates as $devicetemplate): ?>
            <tr>
                <td><?= $this->Number->format($devicetemplate->PK_DeviceTemplate) ?></td>
                <td><?= h($devicetemplate->Description) ?></td>
                <td><?= $devicetemplate->has('devicecategory') ? $this->Html->link($devicetemplate->devicecategory->PK_DeviceCategory, ['controller' => 'Devicecategories', 'action' => 'view', $devicetemplate->devicecategory->PK_DeviceCategory]) : '' ?></td>
                <td><?= $this->Number->format($devicetemplate->FK_Manufacturer) ?></td>
                <td><?= h($devicetemplate->Define) ?></td>
                <td><?= $this->Number->format($devicetemplate->ImplementsDCE) ?></td>
                <td><?= h($devicetemplate->IsEmbedded) ?></td>
                <td><?= h($devicetemplate->CommandLine) ?></td>
                <td><?= $this->Number->format($devicetemplate->IsPlugAndPlay) ?></td>
                <td><?= $this->Number->format($devicetemplate->IsIPBased) ?></td>
                <td><?= $this->Number->format($devicetemplate->IsPlugIn) ?></td>
                <td><?= $this->Number->format($devicetemplate->IRFrequency) ?></td>
                <td><?= $this->Number->format($devicetemplate->FK_StabilityStatus) ?></td>
                <td><?= $this->Number->format($devicetemplate->FK_Package) ?></td>
                <td><?= h($devicetemplate->DestinationDir) ?></td>
                <td><?= $this->Number->format($devicetemplate->FK_Users_Maintainer) ?></td>
                <td><?= $this->Number->format($devicetemplate->FK_OperatingSystem) ?></td>
                <td><?= $this->Number->format($devicetemplate->FK_Distro) ?></td>
                <td><?= h($devicetemplate->InheritsMacFromPC) ?></td>
                <td><?= h($devicetemplate->ConfigureScript) ?></td>
                <td><?= h($devicetemplate->ManufacturerURL) ?></td>
                <td><?= h($devicetemplate->InternalURLSuffix) ?></td>
                <td><?= h($devicetemplate->WizardURL) ?></td>
                <td><?= $this->Number->format($devicetemplate->FK_Licensing) ?></td>
                <td><?= $this->Number->format($devicetemplate->FK_InfraredGroup) ?></td>
                <td><?= $this->Number->format($devicetemplate->FK_CommMethod) ?></td>
                <td><?= $this->Number->format($devicetemplate->FK_PnpLevel) ?></td>
                <td><?= h($devicetemplate->HelpURL) ?></td>
                <td><?= $this->Number->format($devicetemplate->psc_id) ?></td>
                <td><?= $this->Number->format($devicetemplate->psc_batch) ?></td>
                <td><?= $this->Number->format($devicetemplate->psc_user) ?></td>
                <td><?= h($devicetemplate->psc_frozen) ?></td>
                <td><?= h($devicetemplate->psc_mod) ?></td>
                <td><?= $this->Number->format($devicetemplate->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['action' => 'view', $devicetemplate->PK_DeviceTemplate]) ?>
                    <?= $this->Html->link(__('Edit'), ['action' => 'edit', $devicetemplate->PK_DeviceTemplate]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['action' => 'delete', $devicetemplate->PK_DeviceTemplate], ['confirm' => __('Are you sure you want to delete # {0}?', $devicetemplate->PK_DeviceTemplate)]) ?>
                </td>
            </tr>
            <?php endforeach; ?>
        </tbody>
    </table>
    <div class="paginator">
        <ul class="pagination">
            <?= $this->Paginator->first('<< ' . __('first')) ?>
            <?= $this->Paginator->prev('< ' . __('previous')) ?>
            <?= $this->Paginator->numbers() ?>
            <?= $this->Paginator->next(__('next') . ' >') ?>
            <?= $this->Paginator->last(__('last') . ' >>') ?>
        </ul>
        <p><?= $this->Paginator->counter(['format' => __('Page {{page}} of {{pages}}, showing {{current}} record(s) out of {{count}} total')]) ?></p>
    </div>
</div>
