<?php
/**
 * @var \App\View\AppView $this
 * @var \App\Model\Entity\Device $device
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('Edit Device'), ['action' => 'edit', $device->PK_Device]) ?> </li>
        <li><?= $this->Form->postLink(__('Delete Device'), ['action' => 'delete', $device->PK_Device], ['confirm' => __('Are you sure you want to delete # {0}?', $device->PK_Device)]) ?> </li>
        <li><?= $this->Html->link(__('List Devices'), ['action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Device'), ['action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Device Template'), ['controller' => 'Devicetemplates', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Device Template'), ['controller' => 'Devicetemplates', 'action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Room'), ['controller' => 'Rooms', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Room'), ['controller' => 'Rooms', 'action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Device Data'), ['controller' => 'Devicedata', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Device Data'), ['controller' => 'Devicedata', 'action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Device  Device Data'), ['controller' => 'Devicedevicedata', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Device  Device Data'), ['controller' => 'Devicedevicedata', 'action' => 'add']) ?> </li>
    </ul>
</nav>
<div class="devices view large-9 medium-8 columns content">
    <h3><?= h($device->PK_Device) ?></h3>
    <table class="vertical-table">
        <tr>
            <th scope="row"><?= __('Description') ?></th>
            <td><?= h($device->Description) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Device Template') ?></th>
            <td><?= $device->has('device_template') ? $this->Html->link($device->device_template->PK_DeviceTemplate, ['controller' => 'Devicetemplates', 'action' => 'view', $device->device_template->PK_DeviceTemplate]) : '' ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('IPaddress') ?></th>
            <td><?= h($device->IPaddress) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('MACaddress') ?></th>
            <td><?= h($device->MACaddress) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('State') ?></th>
            <td><?= h($device->State) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Status') ?></th>
            <td><?= h($device->Status) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Room') ?></th>
            <td><?= $device->has('room') ? $this->Html->link($device->room->PK_Room, ['controller' => 'Rooms', 'action' => 'view', $device->room->PK_Room]) : '' ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('PK Device') ?></th>
            <td><?= $this->Number->format($device->PK_Device) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Disabled') ?></th>
            <td><?= $this->Number->format($device->Disabled) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Room') ?></th>
            <td><?= $this->Number->format($device->FK_Room) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Installation') ?></th>
            <td><?= $this->Number->format($device->FK_Installation) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK DesignObj') ?></th>
            <td><?= $this->Number->format($device->FK_DesignObj) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Device ControlledVia') ?></th>
            <td><?= $this->Number->format($device->FK_Device_ControlledVia) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('IgnoreOnOff') ?></th>
            <td><?= $this->Number->format($device->IgnoreOnOff) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Device RouteTo') ?></th>
            <td><?= $this->Number->format($device->FK_Device_RouteTo) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('EK LicensedDevice') ?></th>
            <td><?= $this->Number->format($device->EK_LicensedDevice) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Id') ?></th>
            <td><?= $this->Number->format($device->psc_id) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Batch') ?></th>
            <td><?= $this->Number->format($device->psc_batch) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc User') ?></th>
            <td><?= $this->Number->format($device->psc_user) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Restrict') ?></th>
            <td><?= $this->Number->format($device->psc_restrict) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Mod') ?></th>
            <td><?= h($device->psc_mod) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('NeedConfigure') ?></th>
            <td><?= $device->NeedConfigure ? __('Yes') : __('No'); ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('PingTest') ?></th>
            <td><?= $device->PingTest ? __('Yes') : __('No'); ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('IsNewDevice') ?></th>
            <td><?= $device->IsNewDevice ? __('Yes') : __('No'); ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('ManuallyConfigureEA') ?></th>
            <td><?= $device->ManuallyConfigureEA ? __('Yes') : __('No'); ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Registered') ?></th>
            <td><?= $device->Registered ? __('Yes') : __('No'); ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Frozen') ?></th>
            <td><?= $device->psc_frozen ? __('Yes') : __('No'); ?></td>
        </tr>
    </table>
    <div class="related">
        <h4><?= __('Related Devicedata') ?></h4>
        <?php if (!empty($device->device_data)): ?>
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
            <?php foreach ($device->device_data as $deviceData): ?>
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
    <div class="related">
        <h4><?= __('Related Devicedevicedata') ?></h4>
        <?php if (!empty($device->device__device_data)): ?>
        <table cellpadding="0" cellspacing="0">
            <tr>
                <th scope="col"><?= __('FK Device') ?></th>
                <th scope="col"><?= __('FK DeviceData') ?></th>
                <th scope="col"><?= __('IK DeviceData') ?></th>
                <th scope="col"><?= __('Psc Id') ?></th>
                <th scope="col"><?= __('Psc Batch') ?></th>
                <th scope="col"><?= __('Psc User') ?></th>
                <th scope="col"><?= __('Psc Frozen') ?></th>
                <th scope="col"><?= __('Psc Mod') ?></th>
                <th scope="col"><?= __('Psc Restrict') ?></th>
                <th scope="col" class="actions"><?= __('Actions') ?></th>
            </tr>
            <?php foreach ($device->device__device_data as $deviceDeviceData): ?>
            <tr>
                <td><?= h($deviceDeviceData->FK_Device) ?></td>
                <td><?= h($deviceDeviceData->FK_DeviceData) ?></td>
                <td><?= h($deviceDeviceData->IK_DeviceData) ?></td>
                <td><?= h($deviceDeviceData->psc_id) ?></td>
                <td><?= h($deviceDeviceData->psc_batch) ?></td>
                <td><?= h($deviceDeviceData->psc_user) ?></td>
                <td><?= h($deviceDeviceData->psc_frozen) ?></td>
                <td><?= h($deviceDeviceData->psc_mod) ?></td>
                <td><?= h($deviceDeviceData->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['controller' => 'Devicedevicedata', 'action' => 'view', $deviceDeviceData->FK_Device]) ?>
                    <?= $this->Html->link(__('Edit'), ['controller' => 'Devicedevicedata', 'action' => 'edit', $deviceDeviceData->FK_Device]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['controller' => 'Devicedevicedata', 'action' => 'delete', $deviceDeviceData->FK_Device], ['confirm' => __('Are you sure you want to delete # {0}?', $deviceDeviceData->FK_Device)]) ?>
                </td>
            </tr>
            <?php endforeach; ?>
        </table>
        <?php endif; ?>
    </div>
</div>
