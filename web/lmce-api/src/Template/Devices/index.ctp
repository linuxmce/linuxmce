<?php
/**
 * @var \App\View\AppView $this
 * @var \App\Model\Entity\Device[]|\Cake\Collection\CollectionInterface $devices
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('New Device'), ['action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Device Template'), ['controller' => 'Devicetemplates', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device Template'), ['controller' => 'Devicetemplates', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Room'), ['controller' => 'Rooms', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Room'), ['controller' => 'Rooms', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Device Data'), ['controller' => 'Devicedata', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device Data'), ['controller' => 'Devicedata', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Device  Device Data'), ['controller' => 'Devicedevicedata', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device  Device Data'), ['controller' => 'Devicedevicedata', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="devices index large-9 medium-8 columns content">
    <h3><?= __('Devices') ?></h3>
    <table cellpadding="0" cellspacing="0">
        <thead>
            <tr>
                <th scope="col"><?= $this->Paginator->sort('PK_Device') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Disabled') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Room') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Installation') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_DesignObj') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Description') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_DeviceTemplate') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Device_ControlledVia') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IPaddress') ?></th>
                <th scope="col"><?= $this->Paginator->sort('MACaddress') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IgnoreOnOff') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Device_RouteTo') ?></th>
                <th scope="col"><?= $this->Paginator->sort('NeedConfigure') ?></th>
                <th scope="col"><?= $this->Paginator->sort('State') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Status') ?></th>
                <th scope="col"><?= $this->Paginator->sort('PingTest') ?></th>
                <th scope="col"><?= $this->Paginator->sort('EK_LicensedDevice') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IsNewDevice') ?></th>
                <th scope="col"><?= $this->Paginator->sort('ManuallyConfigureEA') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Registered') ?></th>
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
            <?php foreach ($devices as $device): ?>
            <tr>
                <td><?= $this->Number->format($device->PK_Device) ?></td>
                <td><?= $this->Number->format($device->Disabled) ?></td>
                <td><?= $this->Number->format($device->FK_Room) ?></td>
                <td><?= $this->Number->format($device->FK_Installation) ?></td>
                <td><?= $this->Number->format($device->FK_DesignObj) ?></td>
                <td><?= h($device->Description) ?></td>
                <td><?= $device->has('device_template') ? $this->Html->link($device->device_template->PK_DeviceTemplate, ['controller' => 'Devicetemplates', 'action' => 'view', $device->device_template->PK_DeviceTemplate]) : '' ?></td>
                <td><?= $this->Number->format($device->FK_Device_ControlledVia) ?></td>
                <td><?= h($device->IPaddress) ?></td>
                <td><?= h($device->MACaddress) ?></td>
                <td><?= $this->Number->format($device->IgnoreOnOff) ?></td>
                <td><?= $this->Number->format($device->FK_Device_RouteTo) ?></td>
                <td><?= h($device->NeedConfigure) ?></td>
                <td><?= h($device->State) ?></td>
                <td><?= h($device->Status) ?></td>
                <td><?= h($device->PingTest) ?></td>
                <td><?= $this->Number->format($device->EK_LicensedDevice) ?></td>
                <td><?= h($device->IsNewDevice) ?></td>
                <td><?= h($device->ManuallyConfigureEA) ?></td>
                <td><?= h($device->Registered) ?></td>
                <td><?= $this->Number->format($device->psc_id) ?></td>
                <td><?= $this->Number->format($device->psc_batch) ?></td>
                <td><?= $this->Number->format($device->psc_user) ?></td>
                <td><?= h($device->psc_frozen) ?></td>
                <td><?= h($device->psc_mod) ?></td>
                <td><?= $this->Number->format($device->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['action' => 'view', $device->PK_Device]) ?>
                    <?= $this->Html->link(__('Edit'), ['action' => 'edit', $device->PK_Device]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['action' => 'delete', $device->PK_Device], ['confirm' => __('Are you sure you want to delete # {0}?', $device->PK_Device)]) ?>
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
