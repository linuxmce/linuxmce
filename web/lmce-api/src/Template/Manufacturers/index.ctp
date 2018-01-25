<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface[]|\Cake\Collection\CollectionInterface $manufacturers
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('New Manufacturer'), ['action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Device Template'), ['controller' => 'Devicetemplates', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device Template'), ['controller' => 'Devicetemplates', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Devices'), ['controller' => 'Devices', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Device'), ['controller' => 'Devices', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="manufacturers index large-9 medium-8 columns content">
    <h3><?= __('Manufacturers') ?></h3>
    <table cellpadding="0" cellspacing="0">
        <thead>
            <tr>
                <th scope="col"><?= $this->Paginator->sort('PK_Manufacturer') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Description') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IRFrequency') ?></th>
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
            <?php foreach ($manufacturers as $manufacturer): ?>
            <tr>
                <td><?= $this->Number->format($manufacturer->PK_Manufacturer) ?></td>
                <td><?= h($manufacturer->Description) ?></td>
                <td><?= $this->Number->format($manufacturer->IRFrequency) ?></td>
                <td><?= $this->Number->format($manufacturer->psc_id) ?></td>
                <td><?= $this->Number->format($manufacturer->psc_batch) ?></td>
                <td><?= $this->Number->format($manufacturer->psc_user) ?></td>
                <td><?= h($manufacturer->psc_frozen) ?></td>
                <td><?= h($manufacturer->psc_mod) ?></td>
                <td><?= $this->Number->format($manufacturer->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['action' => 'view', $manufacturer->PK_Manufacturer]) ?>
                    <?= $this->Html->link(__('Edit'), ['action' => 'edit', $manufacturer->PK_Manufacturer]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['action' => 'delete', $manufacturer->PK_Manufacturer], ['confirm' => __('Are you sure you want to delete # {0}?', $manufacturer->PK_Manufacturer)]) ?>
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
