<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface[]|\Cake\Collection\CollectionInterface $entertainarea
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('New Entertainarea'), ['action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Room'), ['controller' => 'Rooms', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Room'), ['controller' => 'Rooms', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Command Group'), ['controller' => 'Commandgroup', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Command Group'), ['controller' => 'Commandgroup', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="entertainarea index large-9 medium-8 columns content">
    <h3><?= __('Entertainarea') ?></h3>
    <table cellpadding="0" cellspacing="0">
        <thead>
            <tr>
                <th scope="col"><?= $this->Paginator->sort('PK_EntertainArea') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_Room') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Only1Stream') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Description') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Private') ?></th>
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
            <?php foreach ($entertainarea as $entertainarea): ?>
            <tr>
                <td><?= $this->Number->format($entertainarea->PK_EntertainArea) ?></td>
                <td><?= $entertainarea->has('room') ? $this->Html->link($entertainarea->room->PK_Room, ['controller' => 'Rooms', 'action' => 'view', $entertainarea->room->PK_Room]) : '' ?></td>
                <td><?= h($entertainarea->Only1Stream) ?></td>
                <td><?= h($entertainarea->Description) ?></td>
                <td><?= $this->Number->format($entertainarea->Private) ?></td>
                <td><?= $this->Number->format($entertainarea->FK_FloorplanObjectType) ?></td>
                <td><?= $this->Number->format($entertainarea->psc_id) ?></td>
                <td><?= $this->Number->format($entertainarea->psc_batch) ?></td>
                <td><?= $this->Number->format($entertainarea->psc_user) ?></td>
                <td><?= h($entertainarea->psc_frozen) ?></td>
                <td><?= h($entertainarea->psc_mod) ?></td>
                <td><?= $this->Number->format($entertainarea->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['action' => 'view', $entertainarea->PK_EntertainArea]) ?>
                    <?= $this->Html->link(__('Edit'), ['action' => 'edit', $entertainarea->PK_EntertainArea]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['action' => 'delete', $entertainarea->PK_EntertainArea], ['confirm' => __('Are you sure you want to delete # {0}?', $entertainarea->PK_EntertainArea)]) ?>
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
