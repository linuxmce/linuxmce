<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface $roomtype
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('Edit Roomtype'), ['action' => 'edit', $roomtype->PK_RoomType]) ?> </li>
        <li><?= $this->Form->postLink(__('Delete Roomtype'), ['action' => 'delete', $roomtype->PK_RoomType], ['confirm' => __('Are you sure you want to delete # {0}?', $roomtype->PK_RoomType)]) ?> </li>
        <li><?= $this->Html->link(__('List Roomtypes'), ['action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Roomtype'), ['action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Rooms'), ['controller' => 'Rooms', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Room'), ['controller' => 'Rooms', 'action' => 'add']) ?> </li>
    </ul>
</nav>
<div class="roomtypes view large-9 medium-8 columns content">
    <h3><?= h($roomtype->PK_RoomType) ?></h3>
    <table class="vertical-table">
        <tr>
            <th scope="row"><?= __('Description') ?></th>
            <td><?= h($roomtype->Description) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Define') ?></th>
            <td><?= h($roomtype->Define) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('PK RoomType') ?></th>
            <td><?= $this->Number->format($roomtype->PK_RoomType) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Id') ?></th>
            <td><?= $this->Number->format($roomtype->psc_id) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Batch') ?></th>
            <td><?= $this->Number->format($roomtype->psc_batch) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc User') ?></th>
            <td><?= $this->Number->format($roomtype->psc_user) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Restrict') ?></th>
            <td><?= $this->Number->format($roomtype->psc_restrict) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Mod') ?></th>
            <td><?= h($roomtype->psc_mod) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Frozen') ?></th>
            <td><?= $roomtype->psc_frozen ? __('Yes') : __('No'); ?></td>
        </tr>
    </table>
    <div class="related">
        <h4><?= __('Related Rooms') ?></h4>
        <?php if (!empty($roomtype->rooms)): ?>
        <table cellpadding="0" cellspacing="0">
            <tr>
                <th scope="col"><?= __('PK Room') ?></th>
                <th scope="col"><?= __('FK Installation') ?></th>
                <th scope="col"><?= __('FK RoomType') ?></th>
                <th scope="col"><?= __('Description') ?></th>
                <th scope="col"><?= __('FK Icon') ?></th>
                <th scope="col"><?= __('ManuallyConfigureEA') ?></th>
                <th scope="col"><?= __('HideFromOrbiter') ?></th>
                <th scope="col"><?= __('FK FloorplanObjectType') ?></th>
                <th scope="col"><?= __('FloorplanInfo') ?></th>
                <th scope="col"><?= __('Psc Id') ?></th>
                <th scope="col"><?= __('Psc Batch') ?></th>
                <th scope="col"><?= __('Psc User') ?></th>
                <th scope="col"><?= __('Psc Frozen') ?></th>
                <th scope="col"><?= __('Psc Mod') ?></th>
                <th scope="col"><?= __('Psc Restrict') ?></th>
                <th scope="col" class="actions"><?= __('Actions') ?></th>
            </tr>
            <?php foreach ($roomtype->rooms as $rooms): ?>
            <tr>
                <td><?= h($rooms->PK_Room) ?></td>
                <td><?= h($rooms->FK_Installation) ?></td>
                <td><?= h($rooms->FK_RoomType) ?></td>
                <td><?= h($rooms->Description) ?></td>
                <td><?= h($rooms->FK_Icon) ?></td>
                <td><?= h($rooms->ManuallyConfigureEA) ?></td>
                <td><?= h($rooms->HideFromOrbiter) ?></td>
                <td><?= h($rooms->FK_FloorplanObjectType) ?></td>
                <td><?= h($rooms->FloorplanInfo) ?></td>
                <td><?= h($rooms->psc_id) ?></td>
                <td><?= h($rooms->psc_batch) ?></td>
                <td><?= h($rooms->psc_user) ?></td>
                <td><?= h($rooms->psc_frozen) ?></td>
                <td><?= h($rooms->psc_mod) ?></td>
                <td><?= h($rooms->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['controller' => 'Rooms', 'action' => 'view', $rooms->PK_Room]) ?>
                    <?= $this->Html->link(__('Edit'), ['controller' => 'Rooms', 'action' => 'edit', $rooms->PK_Room]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['controller' => 'Rooms', 'action' => 'delete', $rooms->PK_Room], ['confirm' => __('Are you sure you want to delete # {0}?', $rooms->PK_Room)]) ?>
                </td>
            </tr>
            <?php endforeach; ?>
        </table>
        <?php endif; ?>
    </div>
</div>
