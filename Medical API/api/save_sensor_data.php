<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

require_once __DIR__ . '/../includes/db.php';

$json = file_get_contents('php://input');
$data = json_decode($json, true);

if (!$data) {
    echo json_encode(['success' => false, 'error' => 'Invalid JSON']);
    exit;
}

try {
    $stmt = $pdo->query("SELECT patient_id FROM active_session LIMIT 1");
    $session = $stmt->fetch();

    if (!$session) {
        echo json_encode(['success' => false, 'error' => 'No active session']);
        exit;
    }

    $stmt = $pdo->prepare("
        INSERT INTO sensor_readings 
        (patient_id, ecg_value, heart_rate, spo2, body_temp, room_temp, humidity, air_quality)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    ");

    $stmt->execute([
        $session['patient_id'],
        $data['ecg'] ?? 0,
        $data['hr'] ?? 0,
        $data['spo2'] ?? 0,
        $data['bodyTemp'] ?? 0,
        $data['roomTemp'] ?? 0,
        $data['humidity'] ?? 0,
        $data['airQuality'] ?? 0
    ]);

    echo json_encode(['success' => true]);
} catch (Exception $e) {
    echo json_encode(['success' => false, 'error' => $e->getMessage()]);
}
?>
