<?php
namespace App\Test\TestCase\Controller;

use App\Controller\ManufacturersController;
use Cake\TestSuite\IntegrationTestCase;

/**
 * App\Controller\ManufacturersController Test Case
 */
class ManufacturersControllerTest extends IntegrationTestCase
{

    /**
     * Fixtures
     *
     * @var array
     */
    public $fixtures = [
        'app.manufacturers',
        'app.devices',
        'app.room',
        'app.room_type',
        'app.rooms',
        'app.device_template',
        'app.devicecategory',
        'app.manufacturer',
        'app.device_data',
        'app.device_template_device_data',
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
