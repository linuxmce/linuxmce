<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface[]|\Cake\Collection\CollectionInterface $rooms
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('New Room'), ['action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Room Type'), ['controller' => 'Roomtypes', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Room Type'), ['controller' => 'Roomtypes', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Entertain Area'), ['controller' => 'Entertainarea', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Entertain Area'), ['controller' => 'Entertainarea', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Commandgroup'), ['controller' => 'Commandgroup', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Commandgroup'), ['controller' => 'Commandgroup', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="rooms index large-9 medium-8 columns content">
    <h3><?= __('Rooms') ?></h3>
    <table cellpadding="0" cellspacing="0">
        <thead>
            <tr>
                <th scope="col"><?= $this->Paginator->sort('PK_Room') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Installation') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_RoomType') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Description') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Icon') ?></th>
                <th scope="col"><?= $this->Paginator->sort('ManuallyConfigureEA') ?></th>
                <th scope="col"><?= $this->Paginator->sort('HideFromOrbiter') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_FloorplanObjectType') ?></th>
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
            <?php foreach ($rooms as $room): ?>
            <tr>
                <td><?= $this->Number->format($room->PK_Room) ?></td>
                <td><?= $this->Number->format($room->FK_Installation) ?></td>
                <td><?= $room->has('room_type') ? $this->Html->link($room->room_type->PK_RoomType, ['controller' => 'Roomtypes', 'action' => 'view', $room->room_type->PK_RoomType]) : '' ?></td>
                <td><?= h($room->Description) ?></td>
                <td><?= $this->Number->format($room->FK_Icon) ?></td>
                <td><?= h($room->ManuallyConfigureEA) ?></td>
                <td><?= h($room->HideFromOrbiter) ?></td>
                <td><?= $this->Number->format($room->FK_FloorplanObjectType) ?></td>
                <td><?= $this->Number->format($room->psc_id) ?></td>
                <td><?= $this->Number->format($room->psc_batch) ?></td>
                <td><?= $this->Number->format($room->psc_user) ?></td>
                <td><?= h($room->psc_frozen) ?></td>
                <td><?= h($room->psc_mod) ?></td>
                <td><?= $this->Number->format($room->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['action' => 'view', $room->PK_Room]) ?>
                    <?= $this->Html->link(__('Edit'), ['action' => 'edit', $room->PK_Room]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['action' => 'delete', $room->PK_Room], ['confirm' => __('Are you sure you want to delete # {0}?', $room->PK_Room)]) ?>
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
