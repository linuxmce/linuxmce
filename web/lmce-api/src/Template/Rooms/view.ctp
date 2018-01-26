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
        <li><?= $this->Html->link(__('List Entertain Area'), ['controller' => 'Entertainarea', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Entertain Area'), ['controller' => 'Entertainarea', 'action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Commandgroup'), ['controller' => 'Commandgroup', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Commandgroup'), ['controller' => 'Commandgroup', 'action' => 'add']) ?> </li>
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
    <div class="related">
        <h4><?= __('Related Commandgroup') ?></h4>
        <?php if (!empty($room->commandgroup)): ?>
        <table cellpadding="0" cellspacing="0">
            <tr>
                <th scope="col"><?= __('PK CommandGroup') ?></th>
                <th scope="col"><?= __('FK Array') ?></th>
                <th scope="col"><?= __('FK Installation') ?></th>
                <th scope="col"><?= __('Description') ?></th>
                <th scope="col"><?= __('Hint') ?></th>
                <th scope="col"><?= __('CanTurnOff') ?></th>
                <th scope="col"><?= __('AlwaysShow') ?></th>
                <th scope="col"><?= __('CanBeHidden') ?></th>
                <th scope="col"><?= __('FK Criteria Orbiter') ?></th>
                <th scope="col"><?= __('FK DesignObj') ?></th>
                <th scope="col"><?= __('FK Template') ?></th>
                <th scope="col"><?= __('AltID') ?></th>
                <th scope="col"><?= __('FK Icon') ?></th>
                <th scope="col"><?= __('AutoGeneratedDate') ?></th>
                <th scope="col"><?= __('Disabled') ?></th>
                <th scope="col"><?= __('TemplateParm1') ?></th>
                <th scope="col"><?= __('TemplateParm2') ?></th>
                <th scope="col"><?= __('FK Text') ?></th>
                <th scope="col"><?= __('Psc Id') ?></th>
                <th scope="col"><?= __('Psc Batch') ?></th>
                <th scope="col"><?= __('Psc User') ?></th>
                <th scope="col"><?= __('Psc Frozen') ?></th>
                <th scope="col"><?= __('Psc Mod') ?></th>
                <th scope="col"><?= __('Psc Restrict') ?></th>
                <th scope="col" class="actions"><?= __('Actions') ?></th>
            </tr>
            <?php foreach ($room->commandgroup as $commandgroup): ?>
            <tr>
                <td><?= h($commandgroup->PK_CommandGroup) ?></td>
                <td><?= h($commandgroup->FK_Array) ?></td>
                <td><?= h($commandgroup->FK_Installation) ?></td>
                <td><?= h($commandgroup->Description) ?></td>
                <td><?= h($commandgroup->Hint) ?></td>
                <td><?= h($commandgroup->CanTurnOff) ?></td>
                <td><?= h($commandgroup->AlwaysShow) ?></td>
                <td><?= h($commandgroup->CanBeHidden) ?></td>
                <td><?= h($commandgroup->FK_Criteria_Orbiter) ?></td>
                <td><?= h($commandgroup->FK_DesignObj) ?></td>
                <td><?= h($commandgroup->FK_Template) ?></td>
                <td><?= h($commandgroup->AltID) ?></td>
                <td><?= h($commandgroup->FK_Icon) ?></td>
                <td><?= h($commandgroup->AutoGeneratedDate) ?></td>
                <td><?= h($commandgroup->Disabled) ?></td>
                <td><?= h($commandgroup->TemplateParm1) ?></td>
                <td><?= h($commandgroup->TemplateParm2) ?></td>
                <td><?= h($commandgroup->FK_Text) ?></td>
                <td><?= h($commandgroup->psc_id) ?></td>
                <td><?= h($commandgroup->psc_batch) ?></td>
                <td><?= h($commandgroup->psc_user) ?></td>
                <td><?= h($commandgroup->psc_frozen) ?></td>
                <td><?= h($commandgroup->psc_mod) ?></td>
                <td><?= h($commandgroup->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['controller' => 'Commandgroup', 'action' => 'view', $commandgroup->PK_CommandGroup]) ?>
                    <?= $this->Html->link(__('Edit'), ['controller' => 'Commandgroup', 'action' => 'edit', $commandgroup->PK_CommandGroup]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['controller' => 'Commandgroup', 'action' => 'delete', $commandgroup->PK_CommandGroup], ['confirm' => __('Are you sure you want to delete # {0}?', $commandgroup->PK_CommandGroup)]) ?>
                </td>
            </tr>
            <?php endforeach; ?>
        </table>
        <?php endif; ?>
    </div>
    <div class="related">
        <h4><?= __('Related Entertainarea') ?></h4>
        <?php if (!empty($room->entertain_area)): ?>
        <table cellpadding="0" cellspacing="0">
            <tr>
                <th scope="col"><?= __('PK EntertainArea') ?></th>
                <th scope="col"><?= __('FK Room') ?></th>
                <th scope="col"><?= __('Only1Stream') ?></th>
                <th scope="col"><?= __('Description') ?></th>
                <th scope="col"><?= __('Private') ?></th>
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
            <?php foreach ($room->entertain_area as $entertainArea): ?>
            <tr>
                <td><?= h($entertainArea->PK_EntertainArea) ?></td>
                <td><?= h($entertainArea->FK_Room) ?></td>
                <td><?= h($entertainArea->Only1Stream) ?></td>
                <td><?= h($entertainArea->Description) ?></td>
                <td><?= h($entertainArea->Private) ?></td>
                <td><?= h($entertainArea->FK_FloorplanObjectType) ?></td>
                <td><?= h($entertainArea->FloorplanInfo) ?></td>
                <td><?= h($entertainArea->psc_id) ?></td>
                <td><?= h($entertainArea->psc_batch) ?></td>
                <td><?= h($entertainArea->psc_user) ?></td>
                <td><?= h($entertainArea->psc_frozen) ?></td>
                <td><?= h($entertainArea->psc_mod) ?></td>
                <td><?= h($entertainArea->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['controller' => 'Entertainarea', 'action' => 'view', $entertainArea->PK_EntertainArea]) ?>
                    <?= $this->Html->link(__('Edit'), ['controller' => 'Entertainarea', 'action' => 'edit', $entertainArea->PK_EntertainArea]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['controller' => 'Entertainarea', 'action' => 'delete', $entertainArea->PK_EntertainArea], ['confirm' => __('Are you sure you want to delete # {0}?', $entertainArea->PK_EntertainArea)]) ?>
                </td>
            </tr>
            <?php endforeach; ?>
        </table>
        <?php endif; ?>
    </div>
</div>
