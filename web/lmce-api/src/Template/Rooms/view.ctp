<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface $room
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('Edit Room'), ['action' => 'edit', $room->PK_Room]) ?> </li>
        <li><?= $this->Form->postLink(__('Delete Room'), ['action' => 'delete', $room->PK_Room], ['confirm' => __('Are you sure you want to delete # {0}?', $room->PK_Room)]) ?> </li>
        <li><?= $this->Html->link(__('List Rooms'), ['action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Room'), ['action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Room Type'), ['controller' => 'Roomtypes', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Room Type'), ['controller' => 'Roomtypes', 'action' => 'add']) ?> </li>
    </ul>
</nav>
<div class="rooms view large-9 medium-8 columns content">
    <h3><?= h($room->PK_Room) ?></h3>
    <table class="vertical-table">
        <tr>
            <th scope="row"><?= __('Room Type') ?></th>
            <td><?= $room->has('room_type') ? $this->Html->link($room->room_type->PK_RoomType, ['controller' => 'Roomtypes', 'action' => 'view', $room->room_type->PK_RoomType]) : '' ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Description') ?></th>
            <td><?= h($room->Description) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('PK Room') ?></th>
            <td><?= $this->Number->format($room->PK_Room) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Installation') ?></th>
            <td><?= $this->Number->format($room->FK_Installation) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK Icon') ?></th>
            <td><?= $this->Number->format($room->FK_Icon) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK FloorplanObjectType') ?></th>
            <td><?= $this->Number->format($room->FK_FloorplanObjectType) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Id') ?></th>
            <td><?= $this->Number->format($room->psc_id) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Batch') ?></th>
            <td><?= $this->Number->format($room->psc_batch) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc User') ?></th>
            <td><?= $this->Number->format($room->psc_user) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Restrict') ?></th>
            <td><?= $this->Number->format($room->psc_restrict) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Mod') ?></th>
            <td><?= h($room->psc_mod) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('ManuallyConfigureEA') ?></th>
            <td><?= $room->ManuallyConfigureEA ? __('Yes') : __('No'); ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('HideFromOrbiter') ?></th>
            <td><?= $room->HideFromOrbiter ? __('Yes') : __('No'); ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Frozen') ?></th>
            <td><?= $room->psc_frozen ? __('Yes') : __('No'); ?></td>
        </tr>
    </table>
    <div class="row">
        <h4><?= __('FloorplanInfo') ?></h4>
        <?= $this->Text->autoParagraph(h($room->FloorplanInfo)); ?>
    </div>
</div>
