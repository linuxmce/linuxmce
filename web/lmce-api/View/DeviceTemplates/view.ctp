<div class="deviceTemplates view">
<h2><?php echo __('Device Template'); ?></h2>
	<dl>
		<dt><?php echo __('PK DeviceTemplate'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['PK_DeviceTemplate']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Description'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['Description']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Comments'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['Comments']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK DeviceCategory'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_DeviceCategory']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK Manufacturer'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_Manufacturer']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Define'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['Define']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('ImplementsDCE'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['ImplementsDCE']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('IsEmbedded'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['IsEmbedded']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('CommandLine'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['CommandLine']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('IsPlugAndPlay'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['IsPlugAndPlay']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('IsIPBased'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['IsIPBased']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('IsPlugIn'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['IsPlugIn']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('IRFrequency'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['IRFrequency']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK StabilityStatus'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_StabilityStatus']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK Package'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_Package']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('DestinationDir'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['DestinationDir']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK Users Maintainer'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_Users_Maintainer']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK OperatingSystem'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_OperatingSystem']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK Distro'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_Distro']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('InheritsMacFromPC'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['InheritsMacFromPC']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('ConfigureScript'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['ConfigureScript']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('ManufacturerURL'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['ManufacturerURL']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('InternalURLSuffix'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['InternalURLSuffix']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('WizardURL'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['WizardURL']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK Licensing'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_Licensing']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK InfraredGroup'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_InfraredGroup']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK CommMethod'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_CommMethod']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK PnpLevel'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['FK_PnpLevel']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('HelpURL'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['HelpURL']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc Id'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['psc_id']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc Batch'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['psc_batch']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc User'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['psc_user']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc Frozen'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['psc_frozen']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc Mod'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['psc_mod']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc Restrict'); ?></dt>
		<dd>
			<?php echo h($deviceTemplate['DeviceTemplate']['psc_restrict']); ?>
			&nbsp;
		</dd>
	</dl>
</div>
<div class="actions">
	<h3><?php echo __('Actions'); ?></h3>
	<ul>
		<li><?php echo $this->Html->link(__('Edit Device Template'), array('action' => 'edit', $deviceTemplate['DeviceTemplate']['PK_DeviceTemplate'])); ?> </li>
		<li><?php echo $this->Form->postLink(__('Delete Device Template'), array('action' => 'delete', $deviceTemplate['DeviceTemplate']['PK_DeviceTemplate']), array('confirm' => __('Are you sure you want to delete # %s?', $deviceTemplate['DeviceTemplate']['PK_DeviceTemplate']))); ?> </li>
		<li><?php echo $this->Html->link(__('List Device Templates'), array('action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device Template'), array('action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Design Objs'), array('controller' => 'design_objs', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Design Obj'), array('controller' => 'design_objs', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Device Command Groups'), array('controller' => 'device_command_groups', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device Command Group'), array('controller' => 'device_command_groups', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Device Data'), array('controller' => 'device_data', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device Data'), array('controller' => 'device_data', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Events'), array('controller' => 'events', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Event'), array('controller' => 'events', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Infrared Groups'), array('controller' => 'infrared_groups', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Infrared Group'), array('controller' => 'infrared_groups', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Media Types'), array('controller' => 'media_types', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Media Type'), array('controller' => 'media_types', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Media Type Design Objs'), array('controller' => 'media_type_design_objs', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Media Type Design Obj'), array('controller' => 'media_type_design_objs', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Page Setups'), array('controller' => 'page_setups', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Page Setup'), array('controller' => 'page_setups', 'action' => 'add')); ?> </li>
	</ul>
</div>
<div class="related">
	<h3><?php echo __('Related Design Objs'); ?></h3>
	<?php if (!empty($deviceTemplate['DesignObj'])): ?>
	<table cellpadding = "0" cellspacing = "0">
	<tr>
		<th><?php echo __('PK DesignObj'); ?></th>
		<th><?php echo __('Description'); ?></th>
		<th><?php echo __('Define'); ?></th>
		<th><?php echo __('FK DesignObjType'); ?></th>
		<th><?php echo __('FK DesignObjCategory'); ?></th>
		<th><?php echo __('FK DesignObj IncludeIfOtherIncluded'); ?></th>
		<th><?php echo __('Priority'); ?></th>
		<th><?php echo __('KeepGraphicInCache'); ?></th>
		<th><?php echo __('AlwaysInclude'); ?></th>
		<th><?php echo __('CantGoBack'); ?></th>
		<th><?php echo __('CommandsProcessedAtServer'); ?></th>
		<th><?php echo __('TimeoutSeconds'); ?></th>
		<th><?php echo __('Animate'); ?></th>
		<th><?php echo __('FK StabilityStatus'); ?></th>
		<th><?php echo __('CanGoBackToSameScreen'); ?></th>
		<th><?php echo __('ScreenType'); ?></th>
		<th><?php echo __('FK DesignObj SubstForSkin'); ?></th>
		<th><?php echo __('FK Skin SubstForSkin'); ?></th>
		<th><?php echo __('Psc Id'); ?></th>
		<th><?php echo __('Psc Batch'); ?></th>
		<th><?php echo __('Psc User'); ?></th>
		<th><?php echo __('Psc Frozen'); ?></th>
		<th><?php echo __('Psc Mod'); ?></th>
		<th><?php echo __('Psc Restrict'); ?></th>
		<th class="actions"><?php echo __('Actions'); ?></th>
	</tr>
	<?php foreach ($deviceTemplate['DesignObj'] as $designObj): ?>
		<tr>
			<td><?php echo $designObj['PK_DesignObj']; ?></td>
			<td><?php echo $designObj['Description']; ?></td>
			<td><?php echo $designObj['Define']; ?></td>
			<td><?php echo $designObj['FK_DesignObjType']; ?></td>
			<td><?php echo $designObj['FK_DesignObjCategory']; ?></td>
			<td><?php echo $designObj['FK_DesignObj_IncludeIfOtherIncluded']; ?></td>
			<td><?php echo $designObj['Priority']; ?></td>
			<td><?php echo $designObj['KeepGraphicInCache']; ?></td>
			<td><?php echo $designObj['AlwaysInclude']; ?></td>
			<td><?php echo $designObj['CantGoBack']; ?></td>
			<td><?php echo $designObj['CommandsProcessedAtServer']; ?></td>
			<td><?php echo $designObj['TimeoutSeconds']; ?></td>
			<td><?php echo $designObj['Animate']; ?></td>
			<td><?php echo $designObj['FK_StabilityStatus']; ?></td>
			<td><?php echo $designObj['CanGoBackToSameScreen']; ?></td>
			<td><?php echo $designObj['ScreenType']; ?></td>
			<td><?php echo $designObj['FK_DesignObj_SubstForSkin']; ?></td>
			<td><?php echo $designObj['FK_Skin_SubstForSkin']; ?></td>
			<td><?php echo $designObj['psc_id']; ?></td>
			<td><?php echo $designObj['psc_batch']; ?></td>
			<td><?php echo $designObj['psc_user']; ?></td>
			<td><?php echo $designObj['psc_frozen']; ?></td>
			<td><?php echo $designObj['psc_mod']; ?></td>
			<td><?php echo $designObj['psc_restrict']; ?></td>
			<td class="actions">
				<?php echo $this->Html->link(__('View'), array('controller' => 'design_objs', 'action' => 'view', $designObj['PK_DesignObj'])); ?>
				<?php echo $this->Html->link(__('Edit'), array('controller' => 'design_objs', 'action' => 'edit', $designObj['PK_DesignObj'])); ?>
				<?php echo $this->Form->postLink(__('Delete'), array('controller' => 'design_objs', 'action' => 'delete', $designObj['PK_DesignObj']), array('confirm' => __('Are you sure you want to delete # %s?', $designObj['PK_DesignObj']))); ?>
			</td>
		</tr>
	<?php endforeach; ?>
	</table>
<?php endif; ?>

	<div class="actions">
		<ul>
			<li><?php echo $this->Html->link(__('New Design Obj'), array('controller' => 'design_objs', 'action' => 'add')); ?> </li>
		</ul>
	</div>
</div>
<div class="related">
	<h3><?php echo __('Related Device Command Groups'); ?></h3>
	<?php if (!empty($deviceTemplate['DeviceCommandGroup'])): ?>
	<table cellpadding = "0" cellspacing = "0">
	<tr>
		<th><?php echo __('FK Device'); ?></th>
		<th><?php echo __('FK CommandGroup'); ?></th>
		<th><?php echo __('Sort'); ?></th>
		<th><?php echo __('Psc Id'); ?></th>
		<th><?php echo __('Psc Batch'); ?></th>
		<th><?php echo __('Psc User'); ?></th>
		<th><?php echo __('Psc Frozen'); ?></th>
		<th><?php echo __('Psc Mod'); ?></th>
		<th><?php echo __('Psc Restrict'); ?></th>
		<th class="actions"><?php echo __('Actions'); ?></th>
	</tr>
	<?php foreach ($deviceTemplate['DeviceCommandGroup'] as $deviceCommandGroup): ?>
		<tr>
			<td><?php echo $deviceCommandGroup['FK_Device']; ?></td>
			<td><?php echo $deviceCommandGroup['Group']; ?></td>
			<td><?php echo $deviceCommandGroup['Sort']; ?></td>
			<td><?php echo $deviceCommandGroup['psc_id']; ?></td>
			<td><?php echo $deviceCommandGroup['psc_batch']; ?></td>
			<td><?php echo $deviceCommandGroup['psc_user']; ?></td>
			<td><?php echo $deviceCommandGroup['psc_frozen']; ?></td>
			<td><?php echo $deviceCommandGroup['psc_mod']; ?></td>
			<td><?php echo $deviceCommandGroup['psc_restrict']; ?></td>
			<td class="actions">
				<?php echo $this->Html->link(__('View'), array('controller' => 'device_command_groups', 'action' => 'view', $deviceCommandGroup['FK_Device'])); ?>
				<?php echo $this->Html->link(__('Edit'), array('controller' => 'device_command_groups', 'action' => 'edit', $deviceCommandGroup['FK_Device'])); ?>
				<?php echo $this->Form->postLink(__('Delete'), array('controller' => 'device_command_groups', 'action' => 'delete', $deviceCommandGroup['FK_Device']), array('confirm' => __('Are you sure you want to delete # %s?', $deviceCommandGroup['FK_Device']))); ?>
			</td>
		</tr>
	<?php endforeach; ?>
	</table>
<?php endif; ?>

	<div class="actions">
		<ul>
			<li><?php echo $this->Html->link(__('New Device Command Group'), array('controller' => 'device_command_groups', 'action' => 'add')); ?> </li>
		</ul>
	</div>
</div>
<div class="related">
	<h3><?php echo __('Related Device Data'); ?></h3>
	<?php if (!empty($deviceTemplate['DeviceData'])): ?>
	<table cellpadding = "0" cellspacing = "0">
	<tr>
		<th><?php echo __('PK DeviceData'); ?></th>
		<th><?php echo __('Description'); ?></th>
		<th><?php echo __('Define'); ?></th>
		<th><?php echo __('FK ParameterType'); ?></th>
		<th><?php echo __('Psc Id'); ?></th>
		<th><?php echo __('Psc Batch'); ?></th>
		<th><?php echo __('Psc User'); ?></th>
		<th><?php echo __('Psc Frozen'); ?></th>
		<th><?php echo __('Psc Mod'); ?></th>
		<th><?php echo __('Psc Restrict'); ?></th>
		<th class="actions"><?php echo __('Actions'); ?></th>
	</tr>
	<?php foreach ($deviceTemplate['DeviceData'] as $deviceData): ?>
		<tr>
			<td><?php echo $deviceData['PK_DeviceData']; ?></td>
			<td><?php echo $deviceData['Description']; ?></td>
			<td><?php echo $deviceData['Define']; ?></td>
			<td><?php echo $deviceData['FK_ParameterType']; ?></td>
			<td><?php echo $deviceData['psc_id']; ?></td>
			<td><?php echo $deviceData['psc_batch']; ?></td>
			<td><?php echo $deviceData['psc_user']; ?></td>
			<td><?php echo $deviceData['psc_frozen']; ?></td>
			<td><?php echo $deviceData['psc_mod']; ?></td>
			<td><?php echo $deviceData['psc_restrict']; ?></td>
			<td class="actions">
				<?php echo $this->Html->link(__('View'), array('controller' => 'device_data', 'action' => 'view', $deviceData['PK_DeviceData'])); ?>
				<?php echo $this->Html->link(__('Edit'), array('controller' => 'device_data', 'action' => 'edit', $deviceData['PK_DeviceData'])); ?>
				<?php echo $this->Form->postLink(__('Delete'), array('controller' => 'device_data', 'action' => 'delete', $deviceData['PK_DeviceData']), array('confirm' => __('Are you sure you want to delete # %s?', $deviceData['PK_DeviceData']))); ?>
			</td>
		</tr>
	<?php endforeach; ?>
	</table>
<?php endif; ?>

	<div class="actions">
		<ul>
			<li><?php echo $this->Html->link(__('New Device Data'), array('controller' => 'device_data', 'action' => 'add')); ?> </li>
		</ul>
	</div>
</div>
<div class="related">
	<h3><?php echo __('Related Events'); ?></h3>
	<?php if (!empty($deviceTemplate['Event'])): ?>
	<table cellpadding = "0" cellspacing = "0">
	<tr>
		<th><?php echo __('PK Event'); ?></th>
		<th><?php echo __('FK EventCategory'); ?></th>
		<th><?php echo __('Description'); ?></th>
		<th><?php echo __('Define'); ?></th>
		<th><?php echo __('Log'); ?></th>
		<th><?php echo __('Psc Id'); ?></th>
		<th><?php echo __('Psc Batch'); ?></th>
		<th><?php echo __('Psc User'); ?></th>
		<th><?php echo __('Psc Frozen'); ?></th>
		<th><?php echo __('Psc Mod'); ?></th>
		<th><?php echo __('Psc Restrict'); ?></th>
		<th class="actions"><?php echo __('Actions'); ?></th>
	</tr>
	<?php foreach ($deviceTemplate['Event'] as $event): ?>
		<tr>
			<td><?php echo $event['PK_Event']; ?></td>
			<td><?php echo $event['FK_EventCategory']; ?></td>
			<td><?php echo $event['Description']; ?></td>
			<td><?php echo $event['Define']; ?></td>
			<td><?php echo $event['Log']; ?></td>
			<td><?php echo $event['psc_id']; ?></td>
			<td><?php echo $event['psc_batch']; ?></td>
			<td><?php echo $event['psc_user']; ?></td>
			<td><?php echo $event['psc_frozen']; ?></td>
			<td><?php echo $event['psc_mod']; ?></td>
			<td><?php echo $event['psc_restrict']; ?></td>
			<td class="actions">
				<?php echo $this->Html->link(__('View'), array('controller' => 'events', 'action' => 'view', $event['PK_Event'])); ?>
				<?php echo $this->Html->link(__('Edit'), array('controller' => 'events', 'action' => 'edit', $event['PK_Event'])); ?>
				<?php echo $this->Form->postLink(__('Delete'), array('controller' => 'events', 'action' => 'delete', $event['PK_Event']), array('confirm' => __('Are you sure you want to delete # %s?', $event['PK_Event']))); ?>
			</td>
		</tr>
	<?php endforeach; ?>
	</table>
<?php endif; ?>

	<div class="actions">
		<ul>
			<li><?php echo $this->Html->link(__('New Event'), array('controller' => 'events', 'action' => 'add')); ?> </li>
		</ul>
	</div>
</div>
<div class="related">
	<h3><?php echo __('Related Infrared Groups'); ?></h3>
	<?php if (!empty($deviceTemplate['InfraredGroup'])): ?>
	<table cellpadding = "0" cellspacing = "0">
	<tr>
		<th><?php echo __('PK InfraredGroup'); ?></th>
		<th><?php echo __('FK DeviceCategory'); ?></th>
		<th><?php echo __('FK Manufacturer'); ?></th>
		<th><?php echo __('Description'); ?></th>
		<th><?php echo __('FK CommMethod'); ?></th>
		<th><?php echo __('Psc Id'); ?></th>
		<th><?php echo __('Psc Batch'); ?></th>
		<th><?php echo __('Psc User'); ?></th>
		<th><?php echo __('Psc Frozen'); ?></th>
		<th><?php echo __('Psc Mod'); ?></th>
		<th><?php echo __('Psc Restrict'); ?></th>
		<th class="actions"><?php echo __('Actions'); ?></th>
	</tr>
	<?php foreach ($deviceTemplate['InfraredGroup'] as $infraredGroup): ?>
		<tr>
			<td><?php echo $infraredGroup['PK_InfraredGroup']; ?></td>
			<td><?php echo $infraredGroup['FK_DeviceCategory']; ?></td>
			<td><?php echo $infraredGroup['FK_Manufacturer']; ?></td>
			<td><?php echo $infraredGroup['Description']; ?></td>
			<td><?php echo $infraredGroup['FK_CommMethod']; ?></td>
			<td><?php echo $infraredGroup['psc_id']; ?></td>
			<td><?php echo $infraredGroup['psc_batch']; ?></td>
			<td><?php echo $infraredGroup['psc_user']; ?></td>
			<td><?php echo $infraredGroup['psc_frozen']; ?></td>
			<td><?php echo $infraredGroup['psc_mod']; ?></td>
			<td><?php echo $infraredGroup['psc_restrict']; ?></td>
			<td class="actions">
				<?php echo $this->Html->link(__('View'), array('controller' => 'infrared_groups', 'action' => 'view', $infraredGroup['PK_InfraredGroup'])); ?>
				<?php echo $this->Html->link(__('Edit'), array('controller' => 'infrared_groups', 'action' => 'edit', $infraredGroup['PK_InfraredGroup'])); ?>
				<?php echo $this->Form->postLink(__('Delete'), array('controller' => 'infrared_groups', 'action' => 'delete', $infraredGroup['PK_InfraredGroup']), array('confirm' => __('Are you sure you want to delete # %s?', $infraredGroup['PK_InfraredGroup']))); ?>
			</td>
		</tr>
	<?php endforeach; ?>
	</table>
<?php endif; ?>

	<div class="actions">
		<ul>
			<li><?php echo $this->Html->link(__('New Infrared Group'), array('controller' => 'infrared_groups', 'action' => 'add')); ?> </li>
		</ul>
	</div>
</div>
<div class="related">
	<h3><?php echo __('Related Media Types'); ?></h3>
	<?php if (!empty($deviceTemplate['MediaType'])): ?>
	<table cellpadding = "0" cellspacing = "0">
	<tr>
		<th><?php echo __('PK MediaType'); ?></th>
		<th><?php echo __('Define'); ?></th>
		<th><?php echo __('Description'); ?></th>
		<th><?php echo __('FK DesignObj'); ?></th>
		<th><?php echo __('DCEAware'); ?></th>
		<th><?php echo __('Extensions'); ?></th>
		<th><?php echo __('Subdirectory'); ?></th>
		<th><?php echo __('IsExternalTransmission'); ?></th>
		<th><?php echo __('FK Pipe'); ?></th>
		<th><?php echo __('CanBookmark'); ?></th>
		<th><?php echo __('EK AttributeType DefaultSort'); ?></th>
		<th><?php echo __('Psc Id'); ?></th>
		<th><?php echo __('Psc Batch'); ?></th>
		<th><?php echo __('Psc User'); ?></th>
		<th><?php echo __('Psc Frozen'); ?></th>
		<th><?php echo __('Psc Mod'); ?></th>
		<th><?php echo __('Psc Restrict'); ?></th>
		<th class="actions"><?php echo __('Actions'); ?></th>
	</tr>
	<?php foreach ($deviceTemplate['MediaType'] as $mediaType): ?>
		<tr>
			<td><?php echo $mediaType['PK_MediaType']; ?></td>
			<td><?php echo $mediaType['Define']; ?></td>
			<td><?php echo $mediaType['Description']; ?></td>
			<td><?php echo $mediaType['FK_DesignObj']; ?></td>
			<td><?php echo $mediaType['DCEAware']; ?></td>
			<td><?php echo $mediaType['Extensions']; ?></td>
			<td><?php echo $mediaType['Subdirectory']; ?></td>
			<td><?php echo $mediaType['IsExternalTransmission']; ?></td>
			<td><?php echo $mediaType['FK_Pipe']; ?></td>
			<td><?php echo $mediaType['CanBookmark']; ?></td>
			<td><?php echo $mediaType['EK_AttributeType_DefaultSort']; ?></td>
			<td><?php echo $mediaType['psc_id']; ?></td>
			<td><?php echo $mediaType['psc_batch']; ?></td>
			<td><?php echo $mediaType['psc_user']; ?></td>
			<td><?php echo $mediaType['psc_frozen']; ?></td>
			<td><?php echo $mediaType['psc_mod']; ?></td>
			<td><?php echo $mediaType['psc_restrict']; ?></td>
			<td class="actions">
				<?php echo $this->Html->link(__('View'), array('controller' => 'media_types', 'action' => 'view', $mediaType['PK_MediaType'])); ?>
				<?php echo $this->Html->link(__('Edit'), array('controller' => 'media_types', 'action' => 'edit', $mediaType['PK_MediaType'])); ?>
				<?php echo $this->Form->postLink(__('Delete'), array('controller' => 'media_types', 'action' => 'delete', $mediaType['PK_MediaType']), array('confirm' => __('Are you sure you want to delete # %s?', $mediaType['PK_MediaType']))); ?>
			</td>
		</tr>
	<?php endforeach; ?>
	</table>
<?php endif; ?>

	<div class="actions">
		<ul>
			<li><?php echo $this->Html->link(__('New Media Type'), array('controller' => 'media_types', 'action' => 'add')); ?> </li>
		</ul>
	</div>
</div>
<div class="related">
	<h3><?php echo __('Related Media Type Design Objs'); ?></h3>
	<?php if (!empty($deviceTemplate['MediaTypeDesignObj'])): ?>
	<table cellpadding = "0" cellspacing = "0">
	<tr>
		<th><?php echo __('PK MediaType DesignObj'); ?></th>
		<th><?php echo __('FK MediaType'); ?></th>
		<th><?php echo __('FK Screen'); ?></th>
		<th><?php echo __('FK DesignObj Popup'); ?></th>
		<th><?php echo __('FK Screen FileList'); ?></th>
		<th><?php echo __('FK Screen OSD'); ?></th>
		<th><?php echo __('FK Screen Alt'); ?></th>
		<th><?php echo __('FK Screen Alt OSD'); ?></th>
		<th><?php echo __('FK Screen OSD Speed'); ?></th>
		<th><?php echo __('FK Screen OSD Track'); ?></th>
		<th><?php echo __('Description'); ?></th>
		<th><?php echo __('FK Skin'); ?></th>
		<th><?php echo __('UIVersion'); ?></th>
		<th><?php echo __('Psc Id'); ?></th>
		<th><?php echo __('Psc Batch'); ?></th>
		<th><?php echo __('Psc User'); ?></th>
		<th><?php echo __('Psc Frozen'); ?></th>
		<th><?php echo __('Psc Mod'); ?></th>
		<th><?php echo __('Psc Restrict'); ?></th>
		<th class="actions"><?php echo __('Actions'); ?></th>
	</tr>
	<?php foreach ($deviceTemplate['MediaTypeDesignObj'] as $mediaTypeDesignObj): ?>
		<tr>
			<td><?php echo $mediaTypeDesignObj['PK_MediaType_DesignObj']; ?></td>
			<td><?php echo $mediaTypeDesignObj['FK_MediaType']; ?></td>
			<td><?php echo $mediaTypeDesignObj['FK_Screen']; ?></td>
			<td><?php echo $mediaTypeDesignObj['FK_DesignObj_Popup']; ?></td>
			<td><?php echo $mediaTypeDesignObj['FK_Screen_FileList']; ?></td>
			<td><?php echo $mediaTypeDesignObj['FK_Screen_OSD']; ?></td>
			<td><?php echo $mediaTypeDesignObj['FK_Screen_Alt']; ?></td>
			<td><?php echo $mediaTypeDesignObj['FK_Screen_Alt_OSD']; ?></td>
			<td><?php echo $mediaTypeDesignObj['FK_Screen_OSD_Speed']; ?></td>
			<td><?php echo $mediaTypeDesignObj['FK_Screen_OSD_Track']; ?></td>
			<td><?php echo $mediaTypeDesignObj['Description']; ?></td>
			<td><?php echo $mediaTypeDesignObj['FK_Skin']; ?></td>
			<td><?php echo $mediaTypeDesignObj['UIVersion']; ?></td>
			<td><?php echo $mediaTypeDesignObj['psc_id']; ?></td>
			<td><?php echo $mediaTypeDesignObj['psc_batch']; ?></td>
			<td><?php echo $mediaTypeDesignObj['psc_user']; ?></td>
			<td><?php echo $mediaTypeDesignObj['psc_frozen']; ?></td>
			<td><?php echo $mediaTypeDesignObj['psc_mod']; ?></td>
			<td><?php echo $mediaTypeDesignObj['psc_restrict']; ?></td>
			<td class="actions">
				<?php echo $this->Html->link(__('View'), array('controller' => 'media_type_design_objs', 'action' => 'view', $mediaTypeDesignObj['PK_MediaType_DesignObj'])); ?>
				<?php echo $this->Html->link(__('Edit'), array('controller' => 'media_type_design_objs', 'action' => 'edit', $mediaTypeDesignObj['PK_MediaType_DesignObj'])); ?>
				<?php echo $this->Form->postLink(__('Delete'), array('controller' => 'media_type_design_objs', 'action' => 'delete', $mediaTypeDesignObj['PK_MediaType_DesignObj']), array('confirm' => __('Are you sure you want to delete # %s?', $mediaTypeDesignObj['PK_MediaType_DesignObj']))); ?>
			</td>
		</tr>
	<?php endforeach; ?>
	</table>
<?php endif; ?>

	<div class="actions">
		<ul>
			<li><?php echo $this->Html->link(__('New Media Type Design Obj'), array('controller' => 'media_type_design_objs', 'action' => 'add')); ?> </li>
		</ul>
	</div>
</div>
<div class="related">
	<h3><?php echo __('Related Page Setups'); ?></h3>
	<?php if (!empty($deviceTemplate['PageSetup'])): ?>
	<table cellpadding = "0" cellspacing = "0">
	<tr>
		<th><?php echo __('PK PageSetup'); ?></th>
		<th><?php echo __('FK PageSetup Parent'); ?></th>
		<th><?php echo __('Website'); ?></th>
		<th><?php echo __('OrderNum'); ?></th>
		<th><?php echo __('FK Package'); ?></th>
		<th><?php echo __('Description'); ?></th>
		<th><?php echo __('PageURL'); ?></th>
		<th><?php echo __('ShowInTopMenu'); ?></th>
		<th><?php echo __('Psc Id'); ?></th>
		<th><?php echo __('Psc Batch'); ?></th>
		<th><?php echo __('Psc User'); ?></th>
		<th><?php echo __('Psc Frozen'); ?></th>
		<th><?php echo __('Psc Mod'); ?></th>
		<th><?php echo __('Psc Restrict'); ?></th>
		<th class="actions"><?php echo __('Actions'); ?></th>
	</tr>
	<?php foreach ($deviceTemplate['PageSetup'] as $pageSetup): ?>
		<tr>
			<td><?php echo $pageSetup['PK_PageSetup']; ?></td>
			<td><?php echo $pageSetup['FK_PageSetup_Parent']; ?></td>
			<td><?php echo $pageSetup['Website']; ?></td>
			<td><?php echo $pageSetup['OrderNum']; ?></td>
			<td><?php echo $pageSetup['FK_Package']; ?></td>
			<td><?php echo $pageSetup['Description']; ?></td>
			<td><?php echo $pageSetup['pageURL']; ?></td>
			<td><?php echo $pageSetup['showInTopMenu']; ?></td>
			<td><?php echo $pageSetup['psc_id']; ?></td>
			<td><?php echo $pageSetup['psc_batch']; ?></td>
			<td><?php echo $pageSetup['psc_user']; ?></td>
			<td><?php echo $pageSetup['psc_frozen']; ?></td>
			<td><?php echo $pageSetup['psc_mod']; ?></td>
			<td><?php echo $pageSetup['psc_restrict']; ?></td>
			<td class="actions">
				<?php echo $this->Html->link(__('View'), array('controller' => 'page_setups', 'action' => 'view', $pageSetup['PK_PageSetup'])); ?>
				<?php echo $this->Html->link(__('Edit'), array('controller' => 'page_setups', 'action' => 'edit', $pageSetup['PK_PageSetup'])); ?>
				<?php echo $this->Form->postLink(__('Delete'), array('controller' => 'page_setups', 'action' => 'delete', $pageSetup['PK_PageSetup']), array('confirm' => __('Are you sure you want to delete # %s?', $pageSetup['PK_PageSetup']))); ?>
			</td>
		</tr>
	<?php endforeach; ?>
	</table>
<?php endif; ?>

	<div class="actions">
		<ul>
			<li><?php echo $this->Html->link(__('New Page Setup'), array('controller' => 'page_setups', 'action' => 'add')); ?> </li>
		</ul>
	</div>
</div>
