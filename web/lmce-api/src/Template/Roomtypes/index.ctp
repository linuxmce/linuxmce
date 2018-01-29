<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface[]|\Cake\Collection\CollectionInterface $roomtypes
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('New Roomtype'), ['action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Rooms'), ['controller' => 'Rooms', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Room'), ['controller' => 'Rooms', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="roomtypes index large-9 medium-8 columns content">
    <h3><?= __('Roomtypes') ?></h3>
    <table cellpadding="0" cellspacing="0">
        <thead>
            <tr>
                <th scope="col"><?= $this->Paginator->sort('PK_RoomType') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Description') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Define') ?></th>
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
            <?php foreach ($roomtypes as $roomtype): ?>
            <tr>
                <td><?= $this->Number->format($roomtype->PK_RoomType) ?></td>
                <td><?= h($roomtype->Description) ?></td>
                <td><?= h($roomtype->Define) ?></td>
                <td><?= $this->Number->format($roomtype->psc_id) ?></td>
                <td><?= $this->Number->format($roomtype->psc_batch) ?></td>
                <td><?= $this->Number->format($roomtype->psc_user) ?></td>
                <td><?= h($roomtype->psc_frozen) ?></td>
                <td><?= h($roomtype->psc_mod) ?></td>
                <td><?= $this->Number->format($roomtype->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['action' => 'view', $roomtype->PK_RoomType]) ?>
                    <?= $this->Html->link(__('Edit'), ['action' => 'edit', $roomtype->PK_RoomType]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['action' => 'delete', $roomtype->PK_RoomType], ['confirm' => __('Are you sure you want to delete # {0}?', $roomtype->PK_RoomType)]) ?>
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
