<?php
namespace App\Test\TestCase\Controller;

use App\Controller\DevicesmanufacturersController;
use Cake\TestSuite\IntegrationTestCase;

/**
 * App\Controller\DevicesmanufacturersController Test Case
 */
class DevicesmanufacturersControllerTest extends IntegrationTestCase
{

    /**
     * Fixtures
     *
     * @var array
     */
    public $fixtures = [
        'app.devicesmanufacturers',
        'app.devices',
        'app.room',
        'app.room_type',
        'app.rooms',
        'app.device_template',
        'app.devicecategory',
        'app.manufacturer',
        'app.devices_manufacturers',
        'app.devicetemplates',
        'app.device_data',
        'app.device_template_device_data',
        'app.manufacturers',
        'app.device_device_data'
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
