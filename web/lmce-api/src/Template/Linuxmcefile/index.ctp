<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface[]|\Cake\Collection\CollectionInterface $linuxmcefile
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('New Linuxmcefile'), ['action' => 'add']) ?></li>
    </ul>
</nav>
<div class="linuxmcefile index large-9 medium-8 columns content">
    <h3><?= __('Linuxmcefile') ?></h3>
    <table cellpadding="0" cellspacing="0">
        <thead>
            <tr>
                <th scope="col"><?= $this->Paginator->sort('PK_File') ?></th>
                <th scope="col"><?= $this->Paginator->sort('EK_MediaType') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_MediaSubType') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_FileFormat') ?></th>
                <th scope="col"><?= $this->Paginator->sort('FK_FileGroup') ?></th>
                <th scope="col"><?= $this->Paginator->sort('DateAdded') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Path') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Filename') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Missing') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IsDirectory') ?></th>
                <th scope="col"><?= $this->Paginator->sort('EK_Users_Private') ?></th>
                <th scope="col"><?= $this->Paginator->sort('EK_Device') ?></th>
                <th scope="col"><?= $this->Paginator->sort('ModificationDate') ?></th>
                <th scope="col"><?= $this->Paginator->sort('AttrCount') ?></th>
                <th scope="col"><?= $this->Paginator->sort('AttrDate') ?></th>
                <th scope="col"><?= $this->Paginator->sort('DateLastViewed') ?></th>
                <th scope="col"><?= $this->Paginator->sort('IsNew') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Ignore') ?></th>
                <th scope="col"><?= $this->Paginator->sort('INode') ?></th>
                <th scope="col"><?= $this->Paginator->sort('MD5') ?></th>
                <th scope="col"><?= $this->Paginator->sort('Source') ?></th>
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
            <?php foreach ($linuxmcefile as $linuxmcefile): ?>
            <tr>
                <td><?= $this->Number->format($linuxmcefile->PK_File) ?></td>
                <td><?= $this->Number->format($linuxmcefile->EK_MediaType) ?></td>
                <td><?= $this->Number->format($linuxmcefile->FK_MediaSubType) ?></td>
                <td><?= $this->Number->format($linuxmcefile->FK_FileFormat) ?></td>
                <td><?= $this->Number->format($linuxmcefile->FK_FileGroup) ?></td>
                <td><?= h($linuxmcefile->DateAdded) ?></td>
                <td><?= h($linuxmcefile->Path) ?></td>
                <td><?= h($linuxmcefile->Filename) ?></td>
                <td><?= $this->Number->format($linuxmcefile->Missing) ?></td>
                <td><?= $this->Number->format($linuxmcefile->IsDirectory) ?></td>
                <td><?= $this->Number->format($linuxmcefile->EK_Users_Private) ?></td>
                <td><?= $this->Number->format($linuxmcefile->EK_Device) ?></td>
                <td><?= h($linuxmcefile->ModificationDate) ?></td>
                <td><?= $this->Number->format($linuxmcefile->AttrCount) ?></td>
                <td><?= h($linuxmcefile->AttrDate) ?></td>
                <td><?= h($linuxmcefile->DateLastViewed) ?></td>
                <td><?= h($linuxmcefile->IsNew) ?></td>
                <td><?= h($linuxmcefile->Ignore) ?></td>
                <td><?= $this->Number->format($linuxmcefile->INode) ?></td>
                <td><?= h($linuxmcefile->MD5) ?></td>
                <td><?= h($linuxmcefile->Source) ?></td>
                <td><?= $this->Number->format($linuxmcefile->psc_id) ?></td>
                <td><?= $this->Number->format($linuxmcefile->psc_batch) ?></td>
                <td><?= $this->Number->format($linuxmcefile->psc_user) ?></td>
                <td><?= h($linuxmcefile->psc_frozen) ?></td>
                <td><?= h($linuxmcefile->psc_mod) ?></td>
                <td><?= $this->Number->format($linuxmcefile->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['action' => 'view', $linuxmcefile->PK_File]) ?>
                    <?= $this->Html->link(__('Edit'), ['action' => 'edit', $linuxmcefile->PK_File]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['action' => 'delete', $linuxmcefile->PK_File], ['confirm' => __('Are you sure you want to delete # {0}?', $linuxmcefile->PK_File)]) ?>
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
