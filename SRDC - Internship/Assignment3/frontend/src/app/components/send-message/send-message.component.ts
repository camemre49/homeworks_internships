import {Component, OnInit} from '@angular/core';
import {ApiService} from "../../services/api.service";
import {Message} from "../../../Message";
import {MenuService} from "../../services/menu.service";
import {FormControl} from "@angular/forms";
import {Observable, map, startWith} from "rxjs";

@Component({
  selector: 'app-send-message',
  templateUrl: './send-message.component.html',
  styleUrls: ['./send-message.component.css']
})
export class SendMessageComponent implements OnInit{
  receiver: string;
  title: string;
  message: string;
  date: Date;
  myControl = new FormControl();
  options: string[] = []

  filteredOptions: Observable<string[]>;


  constructor(private apiService: ApiService, private menuService: MenuService) {
  }

  ngOnInit() {
    this.myControl.valueChanges.subscribe(async (value) => {
      if(value.length < 2){
        this.options = [];
      }
      else{
        await this.apiService.getUsers(value,1, "", -1).subscribe( (value: string[]) => {
          this.options = value;
          this.filteredOptions = this.myControl.valueChanges.pipe(
            startWith(""),
            map(value => this.filter(value))
          )
        })
      }
    })
  }

  filter(value: string): string[]{
    const filterValue = value.toLowerCase();
    return this.options.filter(option => option.toLowerCase().includes(filterValue))
  }


  sendMessage() {
    this.receiver = this.myControl.value;

    if (!this.receiver) {
      alert("Receiver cannot be empty!")
      return;
    }
    if(!this.message){
      alert("Message cannot be empty!")
      return;
    }
    if (!this.title) {
      if (confirm("Title is empty, do you want to send?")) {

      } else {
        return;
      }
    }

    let message: Message = {
      receiver: this.receiver,
      title: this.title,
      message: this.message,
      sender: "",
      date: new Date()
    }

    this.apiService.sendMessage(message).subscribe((response) => {
      alert("Message sent!")
      this.message = "";
      this.receiver = "";
      this.title = "";
      this.menuService.turnMenu();
    }
    );
  }

}


