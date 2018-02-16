<?php
namespace App\Test\TestCase\Controller;

use App\Controller\DevicesManufacturersController;
use Cake\TestSuite\IntegrationTestCase;

/**
 * App\Controller\DevicesManufacturersController Test Case
 */
class DevicesManufacturersControllerTest extends IntegrationTestCase
{

    /**
     * Fixtures
     *
     * @var array
     */
    public $fixtures = [
        'app.devices_manufacturers',
        'app.devices',
        'app.room',
        'app.room_type',
        'app.rooms',
        'app.device_template',
        'app.devicecategory',
        'app.manufacturer',
        'app.device_data',
        'app.device_template_device_data',
        'app.device_device_data',
        'app.devicetemplates',
        'app.manufacturers'
    ];

    /**
     * Test index method
     *
     * @return void
     */
    public function testIndex()
    {
        $this->markTestIncomplete('Not implemented yet.');
    }

    /**
     * Test view method
     *
     * @return void
     */
    public function testView()
    {
        $this->markTestIncomplete('Not implemented yet.');
    }

    /**
     * Test add method
     *
     * @return void
     */
    public function testAdd()
    {
        $this->markTestIncomplete('Not implemented yet.');
    }

    /**
     * Test edit method
     *
     * @return void
     */
    public function testEdit()
    {
        $this->markTestIncomplete('Not implemented yet.');
    }

    /**
     * Test delete method
     *
     * @return void
     */
    public function testDelete()
    {
        $this->markTestIncomplete('Not implemented yet.');
    }
}
