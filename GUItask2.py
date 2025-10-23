import tkinter as tk 
from tkinter import messagebox
import subprocess
import threading
import re

# Path to C++ exe
EXECUTABLE = r"C:\Users\hanaa\Desktop\sems 5\advanced algorithms\coursework\task 2\towerOfHanoi.exe"

class TowerOfHanoiVisualizer:
    def __init__(self, root):
        self.root = root
        self.root.title("🌙 Tower of Hanoi Visualizer")
        self.root.configure(bg="#f4f6f8")

        # Control panel
        top = tk.Frame(root, bg="#f4f6f8")
        top.pack(pady=10)

        tk.Label(top, text="Disks:", font=("Segoe UI", 11), bg="#f4f6f8").pack(side=tk.LEFT)
        self.disk_entry = tk.Entry(top, width=5, font=("Segoe UI", 11), justify="center")
        self.disk_entry.insert(0, "3")
        self.disk_entry.pack(side=tk.LEFT, padx=5)

        self.setup_btn = tk.Button(top, text="Setup", bg="#0096c7", fg="white",
                                   font=("Segoe UI", 10, "bold"), width=10, command=self.setup_towers)
        self.setup_btn.pack(side=tk.LEFT, padx=5)

        self.start_btn = tk.Button(top, text="Start", bg="#38b000", fg="white",
                                   font=("Segoe UI", 10, "bold"), width=10,
                                   command=self.toggle_animation, state="disabled")
        self.start_btn.pack(side=tk.LEFT, padx=5)

        self.reset_btn = tk.Button(top, text="Reset", bg="#ff6700", fg="white",
                                   font=("Segoe UI", 10, "bold"), width=10,
                                   command=self.reset_towers)
        self.reset_btn.pack(side=tk.LEFT, padx=5)

        self.quit_btn = tk.Button(top, text="Quit", bg="#d62828", fg="white",
                                  font=("Segoe UI", 10, "bold"), width=10,
                                  command=root.quit)
        self.quit_btn.pack(side=tk.LEFT, padx=5)

        # Speed 
        speed_frame = tk.Frame(root, bg="#f4f6f8")
        speed_frame.pack(pady=5)
        tk.Label(speed_frame, text="Speed:", font=("Segoe UI", 10), bg="#f4f6f8").pack(side=tk.LEFT)
        self.speed_label = tk.Label(speed_frame, text="1x", font=("Segoe UI", 10, "bold"), bg="#f4f6f8")
        self.speed_label.pack(side=tk.LEFT, padx=5)
        tk.Button(speed_frame, text="+", width=3, command=lambda: self.adjust_speed(1)).pack(side=tk.LEFT)
        tk.Button(speed_frame, text="-", width=3, command=lambda: self.adjust_speed(-1)).pack(side=tk.LEFT)

        # Canvas
        self.canvas = tk.Canvas(root, width=700, height=420, bg="#e9ecef", highlightthickness=0)
        self.canvas.pack(padx=20, pady=10)

        # Status
        self.status = tk.Label(root, text="", font=("Segoe UI", 10), bg="#f4f6f8", fg="#444")
        self.status.pack()
        self.move_counter = tk.Label(root, text="", font=("Segoe UI", 10, "bold"), bg="#f4f6f8", fg="#222")
        self.move_counter.pack(pady=2)

        self.running = False
        self.moves = []
        self.move_no = 0
        self.anim_steps = 15
        self.anim_delay = 10
        self.speed_multiplier = 1

    # Setup + Reset
    def setup_towers(self):
        if self.running:
            return  

        self.canvas.delete("all")
        try:
            self.num_disks = int(self.disk_entry.get())
            if not (1 <= self.num_disks <= 12):
                raise ValueError
        except ValueError:
            messagebox.showerror("Invalid input", "Enter a number between 1 and 12.")
            return

        self.disk_entry.config(state="disabled")
        self.start_btn.config(state="disabled")

        self.rods_x = [180, 350, 520]
        self.rod_height = 230
        self.disk_height = 22
        self.margin_bottom = 60

        # Animation adjustments
        self.anim_steps = max(5, min(20, 25 - self.num_disks))
        self.anim_delay = max(5, min(15, 20 - self.num_disks))

        # Base and rods
        self.canvas.create_rectangle(80, 400 - self.margin_bottom, 620, 410 - self.margin_bottom,
                                     fill="#adb5bd", outline="")
        for x in self.rods_x:
            self.canvas.create_rectangle(x - 6, 400 - self.rod_height - self.margin_bottom,
                                         x + 6, 400 - self.margin_bottom, fill="#495057", outline="")

        colors = ["#ff595e", "#ffca3a", "#8ac926", "#1982c4", "#6a4c93",
                  "#ff7b00", "#06d6a0", "#8338ec"]

        self.positions = [list(range(self.num_disks, 0, -1)), [], []]
        self.disk_ids = {}
        for i, size in enumerate(self.positions[0]):
            width = 60 + size * 20
            x = self.rods_x[0]
            y = 400 - self.margin_bottom - (i + 1) * self.disk_height
            disk = self.canvas.create_oval(x - width / 2, y - self.disk_height,
                                           x + width / 2, y, fill=colors[i % len(colors)], outline="")
            self.disk_ids[size] = disk

        self.moves.clear()
        self.move_no = 0
        self.update_move_counter()
        self.status.config(text=f"Setting up {self.num_disks} disks...")

        # Run C++ in a separate thread so they work in parallal
        threading.Thread(target=self.generate_moves_from_cpp, args=(self.num_disks,), daemon=True).start()

    def reset_towers(self):
        if self.running:
            return
        self.disk_entry.config(state="normal")
        self.start_btn.config(state="disabled")
        self.setup_towers()

    # Generate C++ Moves 
    def generate_moves_from_cpp(self, n):
        try:
            result = subprocess.run([EXECUTABLE], input=f"{n}\n", text=True,
                                    capture_output=True, timeout=5)
            if result.returncode != 0:
                raise Exception(result.stderr.strip() or "Unknown error from C++ executable.")

            rod_map = {'A': 0, 'B': 1, 'C': 2}
            pattern = re.compile(r'([ABC])\s+([ABC])', re.IGNORECASE)

            self.moves = []
            positions_copy = [list(stack) for stack in self.positions]

            for raw in result.stdout.splitlines():
                m = pattern.search(raw)
                if not m:
                    continue
                from_r, to_r = m.group(1).upper(), m.group(2).upper()
                from_idx, to_idx = rod_map[from_r], rod_map[to_r]
                if not positions_copy[from_idx]:
                    continue
                disk_number = positions_copy[from_idx][-1]
                positions_copy[to_idx].append(positions_copy[from_idx].pop())
                self.moves.append((disk_number, from_idx, to_idx))

            self.status.config(text=f"Ready with {self.num_disks} disks.")
            self.start_btn.config(state="normal")
        except Exception as e:
            self.status.config(text="Error generating moves.")
            self.disk_entry.config(state="normal")
            messagebox.showerror("Error", f"Failed to run C++ executable:\n{e}")

    # Animation
    def toggle_animation(self):
        if self.running or not self.moves:
            return
        self.running = True
        self.status.config(text="Running...")
        self.animate_next_move(0)

    def animate_next_move(self, index):
        if index >= len(self.moves):
            self.running = False
            self.status.config(text="Completed!")
            self.disk_entry.config(state="normal")
            return
        disk, from_rod, to_rod = self.moves[index]
        self.animate_disk_move(disk, from_rod, to_rod,
                               lambda: self.animate_next_move(index + 1))

    def animate_disk_move(self, disk, from_rod, to_rod, callback):
        if not self.positions[from_rod]:
            callback()
            return
        disk_id = self.disk_ids[disk]
        x_target = self.rods_x[to_rod]
        y_target = 400 - self.margin_bottom - (len(self.positions[to_rod]) + 1) * self.disk_height

        def finalize():
            self.positions[to_rod].append(self.positions[from_rod].pop())
            self.move_no += 1
            self.update_move_counter()
            callback()

        self.lift_disk(disk_id, self.anim_steps,
                       lambda: self.slide_disk(disk_id, x_target, self.anim_steps,
                                               lambda: self.drop_disk(disk_id, y_target, self.anim_steps, finalize)))

    # Disk Movement
    def lift_disk(self, disk_id, steps, callback):
        if steps <= 0:
            callback()
            return
        self.canvas.move(disk_id, 0, -5)  
        self.root.after(int(self.anim_delay / self.speed_multiplier),
                        lambda: self.lift_disk(disk_id, steps - 1, callback))

    def slide_disk(self, disk_id, x_target, steps, callback):
        if steps <= 0:
            callback()
            return
        x0, _, x1, _ = self.canvas.coords(disk_id)
        current_x = (x0 + x1) / 2
        dx = (x_target - current_x) / steps  
        self.canvas.move(disk_id, dx, 0)
        self.root.after(int(self.anim_delay / self.speed_multiplier),
                        lambda: self.slide_disk(disk_id, x_target, steps - 1, callback))

    def drop_disk(self, disk_id, y_target, steps, callback):
        if steps <= 0:
            callback()
            return
        _, _, _, y1 = self.canvas.coords(disk_id)
        dy = (y_target - y1) / steps  
        self.canvas.move(disk_id, 0, dy)
        self.root.after(int(self.anim_delay / self.speed_multiplier),
                        lambda: self.drop_disk(disk_id, y_target, steps - 1, callback))

    # Speed Adjustment
    def adjust_speed(self, delta):
        self.speed_multiplier = max(0.2, min(5, self.speed_multiplier + delta * 0.2))
        self.speed_label.config(text=f"{self.speed_multiplier:.1f}x")

    # Moves Counter
    def update_move_counter(self):
        total_moves = len(self.moves)
        self.move_counter.config(text=f"Moves: {self.move_no} / {total_moves}")

# Call Function
root = tk.Tk()
app = TowerOfHanoiVisualizer(root)
root.mainloop()
