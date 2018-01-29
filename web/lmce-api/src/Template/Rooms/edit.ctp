<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface $room
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Form->postLink(
                __('Delete'),
                ['action' => 'delete', $room->PK_Room],
                ['confirm' => __('Are you sure you want to delete # {0}?', $room->PK_Room)]
            )
        ?></li>
        <li><?= $this->Html->link(__('List Rooms'), ['action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('List Room Type'), ['controller' => 'Roomtypes', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Room Type'), ['controller' => 'Roomtypes', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Entertain Area'), ['controller' => 'Entertainarea', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Entertain Area'), ['controller' => 'Entertainarea', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Commandgroup'), ['controller' => 'Commandgroup', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Commandgroup'), ['controller' => 'Commandgroup', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="rooms form large-9 medium-8 columns content">
    <?= $this->Form->create($room) ?>
    <fieldset>
        <legend><?= __('Edit Room') ?></legend>
        <?php
            echo $this->Form->control('FK_Installation');
            echo $this->Form->control('FK_RoomType', ['options' => $roomType, 'empty' => true]);
            echo $this->Form->control('Description');
            echo $this->Form->control('FK_Icon');
            echo $this->Form->control('ManuallyConfigureEA');
            echo $this->Form->control('HideFromOrbiter');
            echo $this->Form->control('FK_FloorplanObjectType');
            echo $this->Form->control('FloorplanInfo');
            echo $this->Form->control('psc_id');
            echo $this->Form->control('psc_batch');
            echo $this->Form->control('psc_user');
            echo $this->Form->control('psc_frozen');
            echo $this->Form->control('psc_mod');
            echo $this->Form->control('psc_restrict');
            echo $this->Form->control('commandgroup._ids', ['options' => $commandgroup]);
        ?>
    </fieldset>
    <?= $this->Form->button(__('Submit')) ?>
    <?= $this->Form->end() ?>
</div>
