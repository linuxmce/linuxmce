<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface $entertainarea
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('List Entertainarea'), ['action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('List Room'), ['controller' => 'Rooms', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Room'), ['controller' => 'Rooms', 'action' => 'add']) ?></li>
        <li><?= $this->Html->link(__('List Command Group'), ['controller' => 'Commandgroup', 'action' => 'index']) ?></li>
        <li><?= $this->Html->link(__('New Command Group'), ['controller' => 'Commandgroup', 'action' => 'add']) ?></li>
    </ul>
</nav>
<div class="entertainarea form large-9 medium-8 columns content">
    <?= $this->Form->create($entertainarea) ?>
    <fieldset>
        <legend><?= __('Add Entertainarea') ?></legend>
        <?php
            echo $this->Form->control('FK_Room', ['options' => $room]);
            echo $this->Form->control('Only1Stream');
            echo $this->Form->control('Description');
            echo $this->Form->control('Private');
            echo $this->Form->control('FK_FloorplanObjectType');
            echo $this->Form->control('FloorplanInfo');
            echo $this->Form->control('psc_id');
            echo $this->Form->control('psc_batch');
            echo $this->Form->control('psc_user');
            echo $this->Form->control('psc_frozen');
            echo $this->Form->control('psc_mod');
            echo $this->Form->control('psc_restrict');
            echo $this->Form->control('command_group._ids', ['options' => $commandGroup]);
        ?>
    </fieldset>
    <?= $this->Form->button(__('Submit')) ?>
    <?= $this->Form->end() ?>
</div>
